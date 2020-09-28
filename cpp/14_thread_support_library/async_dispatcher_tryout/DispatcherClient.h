#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include "inc/ScopedScheduledCall.h"

namespace TaskExecution {
class DispatcherItf;

class DispatcherClient
{
private:
	class DispatcherScopeBridge;

public:
	explicit DispatcherClient(DispatcherItf& dispatcher);
	DispatcherClient(DispatcherClient&& dispatcherClient);
	DispatcherClient& operator=(DispatcherClient&& other);
	DispatcherClient(const DispatcherClient&) = delete;
	DispatcherClient& operator=(const DispatcherClient&) = delete;
	virtual ~DispatcherClient();

	template <typename Fn>
	auto Call(const Fn& fn) const -> decltype(fn())
	{
        return m_pDispatcher->Call(fn);
	}

	template <typename Clock, typename Duration>
	ScopedScheduledCall CallAt(const std::chrono::time_point<Clock, Duration>& interval, const std::function<void()>& fn)
	{
        return m_pDispatcher->CallAt(interval, fn);
	}

	template <typename Rep, typename Period>
	ScopedScheduledCall CallAfter(const std::chrono::duration<Rep, Period>& interval, const std::function<void()>& fn)
	{
        return m_pDispatcher->CallAfter(interval, fn);
	}

	template <typename Rep, typename Period>
	ScopedScheduledCall CallEvery(const std::chrono::duration<Rep, Period>& interval, const std::function<void()>& fn)
	{
        return m_pDispatcher->CallEvery(interval, fn);
	}

	void Notify(const std::function<void()>& fn) const;
	void CancelNotifications();

	bool IsDispatcherThread() const;
	void Synchronize() const;

    DispatcherItf& GetDispatcher() const;
	
private:
	DispatcherItf* m_pDispatcher;
	std::shared_ptr<DispatcherScopeBridge> m_pScopeBridge;
};

}
