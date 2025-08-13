#pragma once

#include <atomic>
#include <queue>
#include <cassert>
#include <mutex>
#include <thread>
#include "inc/dbgstream.h"

#include "inc/ScheduledCall.h"
#include "inc/DispatcherItf.h"

namespace TaskExecution {

class Dispatcher : public DispatcherItf
{
public:
    explicit Dispatcher(const std::string& name = "", const std::function<void(const std::string&)>& onUnhandledException = [](const std::string&){});

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

    Dispatcher(Dispatcher&&) = delete;
    Dispatcher& operator=(Dispatcher&&) = delete;

	virtual ~Dispatcher() override;

	virtual void Notify(const std::function<void()>& fn) const override;
	virtual void Synchronize() const override;
	virtual void Cancel(const ScheduledCall& call) const override;
	virtual std::chrono::steady_clock::duration TimeUntilNextExecution(const ScheduledCall& call) const override;
	virtual bool IsDispatcherThread() const override;

protected:
	virtual ScheduledCall CallAtSystemClock(const std::chrono::steady_clock::time_point& at, const std::function<void()>& fn) const override;
	virtual ScheduledCall CallAfterSystemClock(const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) const override;
	virtual ScheduledCall CallEverySystemClock(const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) const override;

private:
	struct CallData
	{
		CallData(const ScheduledCall& scheduledCall, const std::chrono::steady_clock::time_point& at, const std::function<void()>& fn);
		CallData(const ScheduledCall& scheduledCall, const std::chrono::steady_clock::time_point& at, const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn);

		ScheduledCall m_scheduledCall;
		std::chrono::steady_clock::time_point m_at;
		bool m_repeat;
		std::chrono::steady_clock::duration m_interval;
		std::function<void()> m_fn;
	};
	
    unsigned int GetCallId() const;
	void Insert(CallData&& call) const;
	void Remove(const ScheduledCall& call) const;
	std::vector<CallData>::iterator Find(const ScheduledCall& call);
	std::function<void()> GetNextFunction();
	void Run(const std::string& threadName);  


	mutable std::mutex m_qMtx;
	mutable std::condition_variable m_cond;
	mutable std::queue<std::function<void()>> m_q;
	mutable std::vector<CallData> m_scheduledCalls;
    mutable std::atomic<unsigned int> m_callId;
	bool m_end;
	std::thread m_thread;
	std::function<void(const std::string&)> m_onUnhandledException;
};

}
