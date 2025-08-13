#pragma once

#include <cassert>
#include <future>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <cpplib/com/apartment.h>
#include <cpplib/performance/cpplib_performance.h>
#include <cpplib/preprocessor/nodiscard.h>
#include <cpplib/types/uuid.h>
#include "details/dispatcher_impl.h"
#include "injected_thread_itf.h"
#include "task.h"

namespace cpp
{
namespace concurrency
{

//--------------------------------------------------------------------------------------------------------------------

struct requires_dispatcher_thread_exception :
    public requires_injected_thread_exception
{
public:
    explicit requires_dispatcher_thread_exception(const std::string& text) :
        requires_injected_thread_exception(text)
    {
    };
};

//--------------------------------------------------------------------------------------------------------------------

struct requires_not_dispatcher_thread_exception :
    requires_not_injected_thread_exception
{
public:
    explicit requires_not_dispatcher_thread_exception(const std::string& text) :
        requires_not_injected_thread_exception(text)
    {
    };
};

//--------------------------------------------------------------------------------------------------------------------

template<typename CTX>
class DispatcherWithContext :
    public InjectedThreadItf
{
public:
    DispatcherWithContext() noexcept :
        m_pimpl
        (
            std::make_unique<details::DispatcherImpl<CTX>>
            (
                std::chrono::steady_clock::duration::max(),
                performance::NullDispatcherPerformance::shared()
            )
        )
    {
    }

    DispatcherWithContext(const std::chrono::steady_clock::duration& required_response_time, std::shared_ptr<performance::DispatcherPerformanceItf> performance_itf) noexcept :
        m_pimpl(std::make_unique<details::DispatcherImpl<CTX>>(required_response_time, performance_itf))
    {
    }

    ~DispatcherWithContext() noexcept
    {
    }

    NO_DISCARD const bool is_injected_thread() const override
    {
        return m_pimpl->is_dispatcher_thread();
    }

    void injected_thread_required(const std::string& message) const override
    {
        if (!m_pimpl->is_dispatcher_thread())
        {
            throw requires_dispatcher_thread_exception(message);
        }
    }

    void not_injected_thread_required(const std::string& message) const override
    {
        if (m_pimpl->is_dispatcher_thread())
        {
            throw requires_not_dispatcher_thread_exception(message);
        }
    }

    virtual void notify(const std::function<void()> fn) const override
    {
        m_pimpl->notify(fn);
    }

    virtual void synchronize() const override
    {
        m_pimpl->synchronize();
    }

    template<typename L>
    auto call(L&& fn) const -> decltype(fn())
    {
        not_injected_thread_required("call cannot be called from dispatcher thread");
        return m_pimpl->call(fn);
    }

    template<typename R, typename... Args>
    auto call(std::function<R(Args...)> func, Args&&... args) -> R
    {
        not_injected_thread_required("call cannot be called from dispatcher thread");
        return m_pimpl->call([&func, &args...]
                             {
                                 return func(std::forward<Args>(args)...);
                             });
    }

    // if you get a discared warning, then consider using notify. Or really use the returned future
    template<typename L>
    NO_DISCARD auto async(L&& fn) const -> std::future<decltype(fn())>
    {
        not_injected_thread_required("async cannot be called from dispatcher thread");
        return m_pimpl->async(fn);
    }

    // if you get a discared warning, then consider using notify. Or really use the returned future
    template<typename ClassType, typename MemFn, typename... Args>
    NO_DISCARD auto async(ClassType& object, const MemFn& fn, Args&&... args) -> std::future<decltype((object.*fn)(std::forward<Args>(args)...))>
    {
        not_injected_thread_required("async cannot be called from dispatcher thread");
        return m_pimpl->async([&object, &fn, &args...]()
        {
            return (object.*fn)(std::forward<Args>(args)...);
        });
    }

    template<typename L>
    void notify(L&& fn) const
    {
        not_injected_thread_required("notify cannot be called from dispatcher thread");
        static_assert(std::is_void_v<decltype(fn())>, "only functions returning void may be scheduled");
        static_assert(noexcept(fn()), "make sure your lambda is noexcept, and that you catch ANY exceptions in your lambda if necessary"); 

        if (is_injected_thread())
        {
            throw requires_not_dispatcher_thread_exception("async cannot be called from dispatcher thread");
        }
        m_pimpl->notify(fn);
    }

    template<typename L>
    NO_DISCARD std::unique_ptr<DispatcherTaskItf> call_every(const std::chrono::steady_clock::duration &duration, L&& fn) const noexcept
    {
        static_assert(std::is_void_v<decltype(fn())>, "only functions returning void may be scheduled");
        static_assert(noexcept(fn()), "make sure your lambda is noexcept, and that you catch ANY exceptions in your lambda if necessary");
        return m_pimpl->schedule_task(duration, fn);
    }

    template<typename L>
    NO_DISCARD std::shared_ptr<DispatcherTaskItf> call_at(const std::chrono::steady_clock::time_point& time, L&& fn) const noexcept
    {
        static_assert(std::is_void_v<decltype((object.*fn)(std::forward<Args>(args)...))>, "only functions returning void may be scheduled (no observable return value)");
        static_assert(noexcept(std::declval(ClassType).fn(args...)), "make sure your lambda is noexcept, and that you catch ANY exceptions in your lambda if necessary");
        return m_pimpl->schedule_task(time, fn);
    }

    template<typename L>
    auto invoke(L&& fn) const -> decltype(fn())
    {
        return m_pimpl->invoke(fn);
    }

    template<typename Fn>
    NO_DISCARD auto start_task(const std::wstring& description, TaskReportingItf& reporting_itf, Fn&& fn)
    {
        return m_pimpl->call([description, &reporting_itf, fn]
        {
            auto task = make_task(description, reporting_itf, fn);
            task->start();
            return task;
        });
    }

    template<typename Fn>
    NO_DISCARD auto start_task(const std::wstring& description, TaskReportingItf& reporting_itf, const std::chrono::steady_clock::duration& cancellation_timeout, Fn&& fn)
    {
        return m_pimpl->call([description, &reporting_itf, cancellation_timeout, fn]
        {

            auto task = make_task(description, reporting_itf, cancellation_timeout, fn);
            task->start();
            return task;
        });
    }

private:
    std::unique_ptr<details::DispatcherImpl<CTX>> m_pimpl;
};

//=================================================================================================================

using Dispatcher = DispatcherWithContext<details::NullDispatcherContext>;
using MTADispatcher = DispatcherWithContext<cpp::com::MultithreadedApartment>;

//=================================================================================================================

} // concurrency
} // cpp
