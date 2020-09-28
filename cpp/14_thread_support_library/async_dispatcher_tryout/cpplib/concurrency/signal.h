// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <atomic>
#include <condition_variable>
#include <initializer_list>
#include <mutex>
#include <cpplib/types/non_copyable.h>
#include <cpplib/types/scope_guard.h>

namespace cpp
{
namespace concurrency
{

class Signal
{
public:
    Signal() :
        m_value{false}
    {
    };

    virtual ~Signal() = default;

    void signal() noexcept
    {
        m_value = true;
        m_value_changed.notify_all();
    }

    void wait() const
    {
        std::unique_lock<std::mutex> lock(m_value_mutex);
        SCOPE_GUARD([this]() { m_value = false; });
        auto pred = [this] { return m_value; };

        if (pred())
        {
            return ;
        }

        m_value_changed.wait(lock);
    }

    const bool try_wait_for(const std::chrono::steady_clock::duration& duration) const noexcept
    {
        std::unique_lock<std::mutex> lock(m_value_mutex);
        SCOPE_GUARD([this]() { m_value = false; });
        auto pred = [this] { return m_value; };

        if (pred())
        {
            return true;
        }
        return m_value_changed.wait_for(lock, duration, pred);
    }

private:
    mutable std::mutex m_value_mutex;
    mutable std::condition_variable m_value_changed;
    mutable bool m_value;
};

} // concurrency
} // cpp
