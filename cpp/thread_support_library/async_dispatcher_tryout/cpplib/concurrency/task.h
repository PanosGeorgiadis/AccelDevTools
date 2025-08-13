// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <future>
#include <string>
#include <cpplib/link_cpplib.h>
#include <cpplib/concurrency/injected_thread_itf.h>
#include <cpplib/exceptions/timeout_exception.h>
#include <cpplib/exceptions/cancelled_exception.h>
#include <cpplib/exceptions/invalid_state_exception.h>
#include <cpplib/preprocessor/nodiscard.h>
#include <cpplib/traits/make_std_function.h>
#include <cpplib/types/interface.h>
#include <cpplib/types/non_copyable.h>
#include <cpplib/types/non_moveable.h>
#include <cpplib/types/null_object.h>
#include <cpplib/types/string_cast.h>
#include <cpplib/concurrency/state_variable.h>
#include <cpplib/types/scope_guard.h>
#include <cpplib/types/cpp_assert.h>
#include <cpplib/units/time.h>

namespace cpp
{
namespace concurrency
{

//-----------------------------------------------------------------------------------------------------------------------------------------

using TaskSignalValueType = unsigned int;

struct TaskSignalValue
{
    static const TaskSignalValueType None = 0;
    static const TaskSignalValueType Cancel = 1;
};

using TaskSignal = StateVariable<TaskSignalValueType>;

//-----------------------------------------------------------------------------------------------------------------------------------------

class TaskReportingItf :
    public Interface
{
public:
    virtual void report_task_started(const std::wstring& description) = 0;
    virtual void report_task_cancelled(const std::wstring& description) = 0;
    virtual void report_task_finished(const std::wstring& description) = 0;
    virtual void report_task_failed(const std::wstring& description) = 0;
    virtual void report_cancellation_timeout(const std::wstring& description) = 0;
};

//-----------------------------------------------------------------------------------------------------------------------------------------

class NullTaskReporting :
    public NullObject<TaskReportingItf, NullTaskReporting>
{
public:
    virtual void report_task_started(const std::wstring&) override {}
    virtual void report_task_cancelled(const std::wstring&) override {}
    virtual void report_task_finished(const std::wstring&) override {}
    virtual void report_task_failed(const std::wstring&) override {}
    virtual void report_cancellation_timeout(const std::wstring&) override {}
};

//-----------------------------------------------------------------------------------------------------------------------------------------

template<typename RetvalType>
class TaskItf :
    public Interface
{
public:
    virtual void start() = 0;
    virtual RetvalType get() = 0;
    virtual void cancel() = 0;
};

//-----------------------------------------------------------------------------------------------------------------------------------------

namespace details
{

enum class TaskState
{
    Idle,
    Starting,
    Running,
    Cancelling,
    Cancelled,
    Failed,
    Stopped
};

template<typename RetvalType>
struct TaskResult
{
    template<typename Fn>
    TaskResult(Fn&& fn) :
        m_value{ fn() }
    {
    }

    RetvalType get() const
    {
        return m_value;
    }

private:
    RetvalType m_value;
};

template<>
struct TaskResult<void>
{
    template<typename Fn>
    TaskResult(Fn&& fn)
    {
        fn();
    }

    void get() const
    {
    };
};

template<typename RetvalType>
class Task :
    public NonCopyable,
    public NonMoveable,
    public TaskItf<RetvalType>
{
public:
    template<typename Fn>
    explicit Task(const std::wstring& description, TaskReportingItf& reporting, Fn&& fn) :
        m_cancellation_timeout{ cpp::max_timeout },
        m_state{ TaskState::Idle },
        m_fn{ fn },
        m_description{ description },
        m_reporting{ reporting }
    {
    }

    template<typename Fn>
    explicit Task(const std::wstring& description, TaskReportingItf& reporting, std::chrono::steady_clock::duration cancellation_timeout, Fn&& fn) :
        m_cancellation_timeout{ cancellation_timeout },
        m_state{ TaskState::Idle },
        m_fn{ fn },
        m_description{ description },
        m_reporting{ reporting }
    {
    }

    ~Task()
    {
        try
        {
            cancel();
        }
        catch (const timeout_exception&)
        {
            m_reporting.report_cancellation_timeout(m_description);
        }
    }

    virtual void start() override
    {
        if (m_state != TaskState::Idle)
        {
            throw invalid_state_exception();
        }

        m_state.set(TaskState::Starting);

        m_future = std::async(std::launch::async, [this]
        {
            m_reporting.report_task_started(m_description);
            m_state.set(TaskState::Running);
            try
            {
                TaskResult<RetvalType> result([this]
                {
                    return m_fn(m_signal);
                });

                // function did not cancel itself
                if (m_signal == TaskSignalValue::Cancel)
                {
                    throw cancelled_exception();
                }
                else
                {
                    m_reporting.report_task_finished(m_description);
                    m_state.set(TaskState::Stopped);
                }

                return result.get();
            }
            catch (const cancelled_exception&)
            {
                m_state.set(TaskState::Cancelled);
                m_reporting.report_task_cancelled(m_description);
                throw;
            }
            catch (...)
            {
                m_reporting.report_task_failed(m_description);
                m_state.set(TaskState::Failed);
                throw;
            }
        });

        m_state.wait_for_any({ TaskState::Running, TaskState::Cancelled, TaskState::Failed });
    }

    virtual RetvalType get() override
    {
        return m_future.get();
    }

    virtual void cancel() override
    {
        if (m_state == TaskState::Running)
        {
            m_signal.set(TaskSignalValue::Cancel);
            m_state.wait_for_any({ TaskState::Cancelled, TaskState::Stopped }, m_cancellation_timeout);
        }
    }

private:
    std::chrono::steady_clock::duration m_cancellation_timeout;
    TaskSignal m_signal;
    StateVariable<TaskState> m_state;
    std::function<RetvalType(const TaskSignal&)> m_fn;
    std::future<RetvalType> m_future;
    std::wstring m_description;
    TaskReportingItf& m_reporting;
};

template<typename RetvalType>
NO_DISCARD std::shared_ptr<TaskItf<RetvalType>> make_task(
    const std::wstring& description, 
    TaskReportingItf& reporting_itf, 
    const std::function<RetvalType(const TaskSignal& signal)>& fn)
{
    return std::make_unique<details::Task<RetvalType>>(description, reporting_itf, fn);
}

template<typename RetvalType, typename TaskSignalType>
NO_DISCARD std::shared_ptr<TaskItf<RetvalType>> make_task(
    const std::wstring& description,
    TaskReportingItf& reporting_itf,
    const std::chrono::steady_clock::duration& cancellation_timeout,
    const std::function<RetvalType(const StateVariable<TaskSignalType>& signal)>& fn)
{
    return std::make_unique<details::Task<RetvalType>>(description, reporting_itf, cancellation_timeout, fn);
}
} // namespace details


  //-------------------------------------------------------------------------------------------------

class TaskReportToInjectedThread :
    public NonCopyable,
    public NonMoveable,
    public TaskReportingItf
{
public:
    TaskReportToInjectedThread() = delete;

    TaskReportToInjectedThread(InjectedThreadItf& injected_thread, TaskReportingItf& reporting) :
        m_injected_thread{ injected_thread },
        m_reporting{ reporting }
    {
    }

    virtual void report_task_started(const std::wstring& description) override
    {
        std::function<void()> fn = [this, description] { m_reporting.report_task_started(description); };
        m_injected_thread.notify(fn);
    }

    virtual void report_task_cancelled(const std::wstring& description) override
    {
        std::function<void()> fn = [this, description] { m_reporting.report_task_cancelled(description); };
        m_injected_thread.notify(fn);
    }

    virtual void report_task_finished(const std::wstring& description) override
    {
        std::function<void()> fn = [this, description] { m_reporting.report_task_finished(description); };
        m_injected_thread.notify(fn);
    }

    virtual void report_task_failed(const std::wstring& description) override
    {
        std::function<void()> fn = [this, description] { m_reporting.report_task_failed(description); };
        m_injected_thread.notify(fn);
    }

    virtual void report_cancellation_timeout(const std::wstring& description) override
    {
        std::function<void()> fn = [this, description] { m_reporting.report_cancellation_timeout(description); };
        m_injected_thread.notify(fn);
    }

private:
    InjectedThreadItf & m_injected_thread;
    TaskReportingItf& m_reporting;

};

//-------------------------------------------------------------------------------------------------

template<typename RetvalType>
class TaskProxy :
    public NonCopyable,
    public NonMoveable,
    public TaskItf<RetvalType>
{
public:
    explicit TaskProxy(const std::shared_ptr<TaskItf<RetvalType>>& task) :
        m_task{ task }
    {
    }

    ~TaskProxy()
    {
        m_task->cancel();
    }

    virtual void start() override
    {
        m_task->start();
    }

    virtual RetvalType get() override
    {
        return m_task->get();
    }

    virtual void cancel()
    {
        m_task->cancel();
    }

private:
    std::shared_ptr<TaskItf<RetvalType>> m_task;
};

//-------------------------------------------------------------------------------------------------

template<typename Fn>
NO_DISCARD auto make_task(const std::wstring& description, TaskReportingItf& reporting_itf, Fn&& fn)
{
    auto std_fn = traits::make_std_function(fn);
    return details::make_task(description, reporting_itf, std_fn);
}

template<typename Fn>
NO_DISCARD auto make_task(const std::wstring& description, TaskReportingItf& reporting_itf, const std::chrono::steady_clock::duration& cancellation_timeout, Fn&& fn )
{
    auto std_fn = traits::make_std_function(fn);
    return details::make_task(description, reporting_itf, cancellation_timeout, std_fn);
}

template<typename Fn>
NO_DISCARD auto start_task(const std::wstring& description, TaskReportingItf& reporting_itf, Fn&& fn)
{
    auto task = make_task(description, reporting_itf, fn);
    task->start();
    return task;
}

template<typename Fn>
NO_DISCARD auto start_task(const std::wstring& description, TaskReportingItf& reporting_itf, const std::chrono::steady_clock::duration& cancellation_timeout, Fn&& fn)
{
    auto task = make_task(description, reporting_itf, cancellation_timeout, fn);
    task->start();
    return task;
}

void ThrowIfCancelled(const TaskSignal& signal);

} // concurrency
} // cpp


