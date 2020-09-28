// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/concurrency/injected_thread_itf.h>
#include <cpplib/types/event_connection.h>
#include <boost/signals2.hpp>

namespace cpp
{
namespace details
{

class BoostSignalConnection final :
    public EventConnectionItf
{
public:
    BoostSignalConnection() = delete;

    virtual ~BoostSignalConnection()
    {
        m_connection.disconnect();
    }

    explicit BoostSignalConnection(const boost::signals2::connection& connection) :
        m_connection{ std::move(connection) }
    {
    }

    virtual void disconnect() override
    {
        m_connection.disconnect();
    }

private:
    boost::signals2::connection m_connection;
};

} // details

template<typename Fn>
class BoostEvent final
{
public:
    std::unique_ptr<EventConnectionItf> connect(concurrency::InjectedThreadItf* injected_thread, const std::function<Fn>& fn)
    {
        m_injected_thread = injected_thread;
        m_injected_thread->injected_thread_required(__FUNCTION__);
        return std::make_unique<details::BoostSignalConnection>(m_signal.connect(fn));
    }

    template<typename... Args>
    void signal(Args&&... args) const
    {
        m_injected_thread->injected_thread_required(__FUNCTION__);
        m_signal(std::forward<Args>(args)...);
    }

private:
    boost::signals2::signal<Fn> m_signal;
    concurrency::InjectedThreadItf* m_injected_thread = &concurrency::NullInjectedThread::instance();
};

} // cpp
