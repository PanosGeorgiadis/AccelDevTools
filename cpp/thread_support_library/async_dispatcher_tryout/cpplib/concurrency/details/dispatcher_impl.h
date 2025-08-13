// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <list>
#include <memory>
#include <queue>
#include <cpplib/concurrency/state_variable.h>
#include <cpplib/concurrency/signal.h>
#include <cpplib/types/interface.h>
#include <cpplib/types/uuid.h>
#include <cpplib/types/unreferenced_variables.h>
#include "dispatcher_task.h"

namespace cpp
{
namespace concurrency
{
namespace details
{

struct NullDispatcherContext
{
};

// used for thread-safe starting
// and detecting errors at startup of dispatcher thread
// e.g. by a thread initialization context throwing an exception
enum class DispatcherState
{
    starting,
    running,
    stopping,
    stopped
};

template<typename CTX>
class DispatcherImpl
{
public:
    DispatcherImpl(const std::chrono::steady_clock::duration& required_response_time, std::shared_ptr<performance::DispatcherPerformanceItf> performance_itf) :
        m_internal_state{ DispatcherState::starting },
        m_required_response_time{ required_response_time },
        m_performance_itf{ std::move(performance_itf) }
    {
        m_dispatch_future = std::async(std::launch::async, [this]()
        {
            main_loop();
        });

        m_internal_state.wait_for(DispatcherState::running);
    }

    ~DispatcherImpl()
    {
        stop();
        m_dispatch_future.get();
    }

    void stop()
    {
        synchronize();
        m_internal_state.set_if_in(DispatcherState::running, DispatcherState::stopping);
        m_wakeup.signal();
        m_internal_state.wait_for(DispatcherState::stopped);
    }

    void synchronize()
    {
        invoke([] {});
    }

    template<typename L>
    auto call(L&& fn) -> decltype(fn())
    {
        auto future = async(fn);
        return future.get();
    }

    template<typename L>
    auto invoke(L&& fn) -> decltype(fn())
    {
        if (is_dispatcher_thread())
        {
            return fn();
        }
        else
        {
            return call(fn);
        }
    }

    template<typename L>
    auto async(L&& fn) -> std::future<decltype(fn())>
    {
        auto task_ptr = std::make_shared<InternalDispatcherTask<decltype(fn())>>(std::chrono::steady_clock::now(), fn);
        queue_task(task_ptr);
        return task_ptr->get_future();
    }

    template<typename L>
    const void notify(L&& fn)
    {
        auto task_ptr = std::make_shared<InternalDispatcherTask<decltype(fn())>>(std::chrono::steady_clock::now(), fn);
        queue_task(task_ptr);
    }

    template<typename L>
    std::unique_ptr<DispatcherTaskItf> schedule_task(const std::chrono::steady_clock::duration& interval, L&& fn) noexcept
    {
        auto task_ptr = std::make_shared<InternalDispatcherTask<decltype(fn())>>(interval, fn);
        queue_task(task_ptr);
        // return a wrapper to client which will cancel task if client releases task
        return std::make_unique<DispatcherTask>(task_ptr);
    }

    template<typename L>
    std::unique_ptr<DispatcherTaskItf> schedule_task(const std::chrono::steady_clock::time_point& time, L&& fn) noexcept
    {
        auto task_ptr = std::make_shared<InternalDispatcherTask<decltype(fn())>>(time, fn);
        queue_task(task_ptr);

        // return a wrapper to client which will cancel task if client releases task
        return std::make_unique<DispatcherTask>(task_ptr);
    }

    bool is_dispatcher_thread() const
    {
        return (m_dispatcher_thread_id == std::this_thread::get_id());
    }

private:
    void queue_task(const std::shared_ptr<InternalDispatcherTaskItf>& task_ptr)
    {

        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue.push(task_ptr);
        m_wakeup.signal();
        
        m_performance_itf->increment_number_of_calls_queued();
        m_performance_itf->report_queue_size(static_cast<unsigned long>(m_queue.size()));
    }

    bool suspend()
    {
        // if no task scheduled wait for a long time before waking up
        auto wait_duration = std::chrono::steady_clock::duration::min();

        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);

            if (m_queue.size() > 0)
            {
                auto run_next_task_at = m_queue.top()->run_at();
                auto now = std::chrono::steady_clock::now();
                if (now < run_next_task_at)
                {
                    // this is the time it takes to wait until the next call is scheduled.
                    wait_duration = run_next_task_at - now;
                }
            }
        }

        // Wait until it is time to run the next call 
        // or until something else has happened.
        m_wakeup.try_wait_for(wait_duration);
        return true;
    }

    void main_loop()
    {
        // check all scheduled tasks that need to be executed
        // pop them off the queue and add to todo list
        // this copy frees up the queue for adding new tasks
        // without having to wait for the execution time of a running task
        std::list<std::shared_ptr<InternalDispatcherTaskItf>> tasks_to_run;

        try
        {
            static_assert(std::is_class<CTX>::value, "dispatcher thread context must be a class");
            // force context to be created
            CTX thread_context;
            unreferenced::variable(thread_context);

            m_dispatcher_thread_id = std::this_thread::get_id();
            m_internal_state = DispatcherState::running;

            // while running and at least one task has been scheduled to run
            while (suspend())
            {
                if (m_internal_state == DispatcherState::stopping) break;

                // pop all tasks from queue that need running,
                // copy to local list so queue can be unlocked and doesn't have to wait on
                // tasks that need running.
                {
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    auto now = std::chrono::steady_clock::now();

                    // pop all tasks ready to run
                    while ((m_queue.size() > 0) && (now >= m_queue.top()->run_at()))
                    {
                        tasks_to_run.push_back(m_queue.top());
                        m_queue.pop();
                    }

                    m_performance_itf->report_queue_size(static_cast<std::uint32_t>(m_queue.size()));
                }

                // loop over all tasks to run
                for (auto task_ptr : tasks_to_run)
                {
                    auto start = std::chrono::steady_clock::now();
                    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(start - task_ptr->run_at());
                    m_performance_itf->report_latency_in_milliseconds(static_cast<std::uint32_t>(latency.count()));

                    // check start of call
                    task_ptr->call();

                    // recurring non-canceled task must be rescheduled
                    if (task_ptr->reschedule())
                    {
                        task_ptr->reset();
                        queue_task(task_ptr);
                    }
                }

                tasks_to_run.clear();
            }

            m_internal_state = DispatcherState::stopped;
        }
        catch (...)
        {
            m_internal_state = DispatcherState::stopped;
            throw;
        }
    }

    std::thread::id m_dispatcher_thread_id;
    std::future<void> m_dispatch_future;

    std::mutex m_queue_mutex;
    std::priority_queue<std::shared_ptr<InternalDispatcherTaskItf>, std::vector<std::shared_ptr<InternalDispatcherTaskItf>>, task_compare> m_queue;

    StateVariable<DispatcherState> m_internal_state;
    Signal m_wakeup;

    const std::chrono::steady_clock::duration m_required_response_time;
    std::shared_ptr<performance::DispatcherPerformanceItf> m_performance_itf;
};

} // details 
} // concurrency 
} // cpp 