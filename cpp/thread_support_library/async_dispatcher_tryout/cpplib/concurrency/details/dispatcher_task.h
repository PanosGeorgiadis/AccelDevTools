// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <chrono>
#include <memory>
#include <cpplib/types/interface.h>
#include <cpplib/types/non_copyable.h>

namespace cpp
{
namespace concurrency
{

struct DispatcherTaskItf :
    public Interface
{
    virtual void cancel() = 0;
};

namespace details
{

struct InternalDispatcherTaskItf :
    public DispatcherTaskItf
{
    virtual const bool recurring() const noexcept = 0;
    virtual const std::chrono::steady_clock::time_point& run_at() const noexcept = 0;
    virtual const std::chrono::steady_clock::duration& interval() const noexcept = 0;
    virtual void call() = 0;
    virtual void reset() = 0;
    virtual const bool reschedule() noexcept = 0;
};

template<typename R>
struct InternalDispatcherTask :
    public InternalDispatcherTaskItf
{
public:
    template<typename L>
    InternalDispatcherTask(const std::chrono::steady_clock::time_point& run_at, const L& fn) noexcept :
        m_run_at{ run_at },
        m_interval{ std::chrono::steady_clock::duration::zero() },
        m_task{ fn },
        m_is_recurring{ false },
        m_is_active{ true }
    {
    }

    template<typename L>
    InternalDispatcherTask(const std::chrono::steady_clock::duration& interval, const L& fn) noexcept :
        m_run_at{ std::chrono::steady_clock::now() },
        m_interval{ interval },
        m_task{ fn },
        m_is_recurring{ true },
        m_is_active{ true }
    {
    }

    virtual void call() override
    {
        if (m_is_active) 
        {
            m_task();
        }
    }

    std::future<R> get_future()
    {
        return m_task.get_future();
    }
    
    virtual const bool recurring() const noexcept override
    {
        return m_is_recurring;
    }

    virtual const std::chrono::steady_clock::time_point& run_at() const noexcept override
    {
        return m_run_at;
    }

    virtual const std::chrono::steady_clock::duration& interval() const noexcept override
    {
        return m_interval;
    }

    virtual void cancel() override
    {
        std::unique_lock<std::mutex> lock(m_cancellation_mutex);
        m_is_active = false;
        m_interval = std::chrono::steady_clock::duration::zero();
    }

    virtual void reset() override
    {
        m_run_at += m_interval;
        m_task.reset();
    }

    virtual const bool reschedule() noexcept override
    {
        return m_is_active && m_is_recurring;
    }

private:
    std::chrono::steady_clock::time_point m_run_at;
    std::chrono::steady_clock::duration m_interval;
    std::packaged_task<R()> m_task;

    mutable std::mutex m_cancellation_mutex;
    std::atomic<bool> m_is_recurring;
    std::atomic<bool> m_is_active;
};

struct task_compare
{
    bool operator()(const std::shared_ptr<InternalDispatcherTaskItf>& lhs, const std::shared_ptr<InternalDispatcherTaskItf>& rhs) const noexcept
    {
        return lhs->run_at() > rhs->run_at();
    }
};

class DispatcherTask final :
    public NonCopyable,
    public DispatcherTaskItf

{
public:
    explicit DispatcherTask(const std::shared_ptr<InternalDispatcherTaskItf>& internal_task) :
        m_internal_task{ internal_task }
    {
    }

    ~DispatcherTask()
    {
        cancel();
    }

    virtual void cancel() override
    {
        m_internal_task->cancel();
    }

private:
    std::shared_ptr<InternalDispatcherTaskItf> m_internal_task;
};

} // details
} // concurrency
} // cpp