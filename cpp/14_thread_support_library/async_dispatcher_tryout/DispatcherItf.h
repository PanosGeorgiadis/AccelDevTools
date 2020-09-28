#pragma once

#include <functional>
#include <cassert>
#include <future>
#include <chrono>

#include "inc/ScheduledCall.h"

namespace TaskExecution {

class DispatcherItf
{
public:
	DispatcherItf(){};
	virtual ~DispatcherItf(){};

	//Non-copyable:
	DispatcherItf(const DispatcherItf&) = delete;
	DispatcherItf& operator=(const DispatcherItf&) = delete;

	virtual void Notify(const std::function<void()>& fn) const = 0;
	virtual void Synchronize() const = 0;
	virtual void Cancel(const ScheduledCall& call) const = 0;
	virtual	std::chrono::steady_clock::duration TimeUntilNextExecution(const ScheduledCall& call) const = 0;
	virtual bool IsDispatcherThread() const = 0;

	template <typename Fn>
	auto Call(const Fn& fn) const -> decltype(fn());

	template<typename Fn>
	auto CallAsync(const Fn& fn) const ->std::future<decltype(fn())>;

	template <typename Clock, typename Duration>
	ScheduledCall CallAt(const std::chrono::time_point<Clock, Duration>& at, const std::function<void()>& fn) const;

	template <typename Rep, typename Period>
	ScheduledCall CallAfter(const std::chrono::duration<Rep, Period>& interval, const std::function<void()>& fn) const;
	
	template <typename Rep, typename Period>
	ScheduledCall CallEvery(const std::chrono::duration<Rep, Period>& interval, const std::function<void()>& fn) const;

protected:	
	virtual ScheduledCall CallAtSystemClock(const std::chrono::steady_clock::time_point& at, const std::function<void()>& fn) const = 0;
	virtual ScheduledCall CallAfterSystemClock(const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) const = 0;
	virtual ScheduledCall CallEverySystemClock(const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) const = 0;
};

template <typename Fn>
auto DispatcherItf::Call(const Fn& fn) const -> decltype(fn())
{
	return CallAsync(fn).get();
}

template<typename Fn>
auto DispatcherItf::CallAsync(const Fn& fn) const -> std::future<decltype(fn())>
{
	assert(!IsDispatcherThread());
	typedef decltype(fn()) return_type;
	// std::packaged_task<void()> is not movable
	// We need to wrap it in a shared_ptr:
	auto pt = std::make_shared<std::packaged_task<return_type()>>(std::move(fn));
	std::future<return_type> future(pt->get_future());
	Notify([this, pt](){(*pt)(); });
	return future;
}

template <typename Clock, typename Duration>
ScheduledCall DispatcherItf::CallAt(const std::chrono::time_point<Clock, Duration>& at, const std::function<void()>& fn) const
{
	return CallAtSystemClock(std::chrono::time_point_cast<std::chrono::steady_clock::duration>(at), fn);
}

template <typename Rep, typename Period>
ScheduledCall DispatcherItf::CallAfter(const std::chrono::duration<Rep, Period>& interval, const std::function<void()>& fn) const
{
	return CallAfterSystemClock(std::chrono::duration_cast<std::chrono::steady_clock::duration>(interval), fn);
}

template <typename Rep, typename Period>
ScheduledCall DispatcherItf::CallEvery(const std::chrono::duration<Rep, Period>& interval, const std::function<void()>& fn) const
{
	return CallEverySystemClock(std::chrono::duration_cast<std::chrono::steady_clock::duration>(interval), fn);
}

}
