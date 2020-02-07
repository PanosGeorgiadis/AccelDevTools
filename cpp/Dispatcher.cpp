// Copyright (c) 2016 by FEI Company
// All rights reserved. This file includes confidential and proprietary information of FEI Company.

#include "stdafx.h"

#include "Fei/Dispatcher.h"
#include "Fei/dbgstream.h"
#include "Fei/stringbuilder.h"
#include "Timing.h"

namespace
{
#pragma pack(push, 8)
	struct ThreadnameInfo
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	};
#pragma pack(pop)

	void SetThreadName(DWORD dwThreadID, const char* threadName)
	{
		ThreadnameInfo info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;

		__try
		{
			const DWORD MS_VC_EXCEPTION = 0x406D1388;
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	void SetThreadName(const char* threadName)
	{
		SetThreadName(GetCurrentThreadId(), threadName);
	}

	std::string GetThreadName(const std::string& name)
	{
        if (name.empty())
            return "Dispatcher Thread ";
		else
			return name;
	}

	void LogUnhandledException(const std::string& what)
	{
		Fei::cdbg << "Dispatcher::Notify(): exception: " << what << "\n";
	}
}

namespace Fei {

Dispatcher::CallData::CallData(const ScheduledCall&  scheduledCall, const std::chrono::steady_clock::time_point& at, const std::function<void()>& fn) :
	m_scheduledCall(scheduledCall),
	m_at(at),
	m_repeat(false),
	m_fn(fn)
{
}

Dispatcher::CallData::CallData(const ScheduledCall& scheduledCall, const std::chrono::steady_clock::time_point& at, const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) :
    m_scheduledCall(scheduledCall),
	m_at(at),
	m_repeat(true),
	m_interval(interval),
	m_fn(fn)
{
}

Dispatcher::Dispatcher(const std::string& threadName, const std::function<void(const std::string&)>& onUnhandledException) :
	m_end(false),
    m_callId(0),
	m_thread([this, threadName]() { Run(GetThreadName(threadName)); }),
	m_onUnhandledException(onUnhandledException)
{
    assert(onUnhandledException);
}

Dispatcher::~Dispatcher() 
{
	if (!Dispatcher::IsDispatcherThread())
	{
		Dispatcher::Notify([this]() { m_end = true; });
		m_thread.join();
	}
}

void Dispatcher::Notify(const std::function<void()>& fn) const
{
	std::lock_guard<std::mutex> lock(m_qMtx);
	m_q.push(fn);
	m_cond.notify_one();
}

void Dispatcher::Synchronize() const
{
	Call([]() {});
}

void Dispatcher::Insert(CallData&& call) const
{
	auto it = std::lower_bound(m_scheduledCalls.begin(), m_scheduledCalls.end(), call, [](const CallData& a, const CallData& b) { return a.m_at > b.m_at; });
	m_scheduledCalls.insert(it, call);
}

void Dispatcher::Remove(const ScheduledCall& call) const
{
	auto it = std::find_if(m_scheduledCalls.begin(), m_scheduledCalls.end(), [this, call](const CallData& cd) { return cd.m_scheduledCall == call; });
	if (it != m_scheduledCalls.end())
		m_scheduledCalls.erase(it);
}

ScheduledCall Dispatcher::CallAtSystemClock(const std::chrono::steady_clock::time_point& at, const std::function<void()>& fn) const
{
	ScheduledCall scheduledCall(*this, GetCallId());
	if (IsDispatcherThread())
		Insert(Dispatcher::CallData(scheduledCall, at, fn));
	else
		Call([this, scheduledCall, at, fn]() { Insert(Dispatcher::CallData(scheduledCall, at, fn)); });

	return scheduledCall;
}

ScheduledCall Dispatcher::CallAfterSystemClock(const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) const
{
	return CallAtSystemClock(std::chrono::steady_clock::now() + interval, fn);
}

ScheduledCall Dispatcher::CallEverySystemClock(const std::chrono::steady_clock::duration& interval, const std::function<void()>& fn) const
{
	assert(interval.count() > 0);

	ScheduledCall scheduledCall(*this, GetCallId());
	auto firstCallAt = std::chrono::steady_clock::now() + interval;
	if (IsDispatcherThread())
		Insert(Dispatcher::CallData(scheduledCall, firstCallAt, interval, fn));
	else
		Call([this, scheduledCall, firstCallAt, interval, fn]() { Insert(Dispatcher::CallData(scheduledCall, firstCallAt, interval, fn)); });

	return scheduledCall;
}

void Dispatcher::Cancel(const ScheduledCall& call) const
{
	if (IsDispatcherThread())
		Remove(call);
	else
		Call([this, call]() { Remove(call); });
}

std::chrono::steady_clock::duration Dispatcher::TimeUntilNextExecution(const ScheduledCall& call) const
{
	auto fn = [this, call]()
	{
		auto it = std::find_if(m_scheduledCalls.begin(), m_scheduledCalls.end(), [this, call](const CallData& cd) { return cd.m_scheduledCall == call; });
		if (it != m_scheduledCalls.end())
			return (it->m_at - std::chrono::steady_clock::now());

		return std::chrono::steady_clock::duration(-1);
	};

	if (IsDispatcherThread())
		return fn();
	else
		return Call([fn]() { return fn(); });
}

bool Dispatcher::IsDispatcherThread() const
{
	return std::this_thread::get_id() == m_thread.get_id();
}

std::function<void()> Dispatcher::GetNextFunction()
{
	std::unique_lock<std::mutex> lock(m_qMtx);
	while (m_q.empty())
	{
		if (!m_scheduledCalls.empty())
		{
			if (WaitUntil(m_cond, lock, m_scheduledCalls.back().m_at) == std::cv_status::timeout)
			{
				CallData call(std::move(m_scheduledCalls.back()));
				m_scheduledCalls.pop_back();
				if (call.m_repeat)
					Insert(CallData(call.m_scheduledCall, call.m_at + call.m_interval, call.m_interval, call.m_fn));
				return call.m_fn;
			}
		}
		else
		{
			m_cond.wait(lock);
		}
	}

	std::function<void()> fn(std::move(m_q.front()));
	m_q.pop();
	return fn;
}

void Dispatcher::Run(const std::string& threadName)
{
	SetThreadName(threadName.c_str());

	while (!m_end)
	{
		try
		{
			GetNextFunction()();
		}
		catch (std::exception& e)
		{	
			m_onUnhandledException(e.what());
		}
		catch (...)
		{
			m_onUnhandledException("unknown exception");
		}
	}
}

unsigned int Dispatcher::GetCallId() const
{
    return m_callId++;
}


} // namespace Fei

namespace
{
	//std::thread.join fix copied from https://connect.microsoft.com/VisualStudio/feedback/details/747145/
#pragma warning(disable:4073) // initializers put in library initialization area
#pragma init_seg(lib)

#if _MSC_VER < 1900
	struct VS2013_threading_fix
	{
		VS2013_threading_fix()
		{
			_Cnd_do_broadcast_at_thread_exit();
		}
	} threading_fix;
#endif

}