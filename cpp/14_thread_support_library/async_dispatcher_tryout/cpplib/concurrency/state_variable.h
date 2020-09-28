// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <atomic>
#include <condition_variable>
#include <initializer_list>
#include <mutex>

#include <cpplib/units/time.h>
#include <cpplib/exceptions/timeout_exception.h>
#include <cpplib/preprocessor/nodiscard.h>
#include <cpplib/traits/static_asserts.h>
#include <cpplib/types/non_copyable.h>
#include <cpplib/types/non_moveable.h>

namespace cpp
{
namespace concurrency
{

static constexpr auto max_wait_duration = 24_hours;

template<typename T>
class StateVariable final:
    public NonMoveable
{
public:
    StateVariable() : 
        m_value{}
    {
    };

    explicit StateVariable(const T& value) :
        m_value{ value }
    {
    }

    virtual ~StateVariable() = default;
    StateVariable(const StateVariable&) = delete;
    
    void set(const T& value) noexcept
    {
        {
            std::unique_lock<std::mutex> lock(m_value_mutex);
            m_value = value;
        }
        m_value_changed.notify_all();
    }

    NO_DISCARD constexpr T get() const noexcept
    {
        std::unique_lock<std::mutex> lock(m_value_mutex);
        return m_value;
    }

    void operator=(const T& value) noexcept
    {
        set(value);
    }

    NO_DISCARD constexpr bool operator==(const T& value) const noexcept
    {
        std::unique_lock<std::mutex> lock(m_value_mutex);
        return (m_value == value);
    }

    NO_DISCARD constexpr bool operator!=(const T& value) const
    {
        return !operator==(value);
    }

    operator T() const noexcept
    {
        return get();
    }

    const bool try_wait_for(const T& value, const std::chrono::steady_clock::duration& duration) const noexcept
    {
        auto pred = [this, value]
        {
            return (m_value == value);
        };

        std::unique_lock<std::mutex> lock(m_value_mutex);
        if (pred())
        {
            return true;
        }
        return m_value_changed.wait_for(lock, duration, pred);
    }

    void wait_for(const T& value, const std::chrono::steady_clock::duration& duration) const
    {
        if (!try_wait_for(value, duration))
        {
            throw timeout_exception();
        }
    }

    void wait_for(const T& value) const
    {
        if (!try_wait_for(value, max_wait_duration))
        {
            throw timeout_exception();
        }
    }

    const bool try_wait_for_any(const std::initializer_list<T>& values, const std::chrono::steady_clock::duration& duration) const noexcept
    {
        auto pred = [this, &values]
        {
            for (auto value : values)
            {
                if (value == m_value)
                {
                    return true;
                }
            }
            return false;
        };

        std::unique_lock<std::mutex> lock(m_value_mutex);

        if (pred())
        {
            return true;
        }

        return m_value_changed.wait_for(lock, duration, pred);
    }

    const bool try_wait_for_any(const std::initializer_list<T>& values) const noexcept
    {
        auto pred = [this, &values]
        {
            for (auto value : values)
            {
                if (value == m_value)
                {
                    return true;
                }
            }
            return false;
        };

        std::unique_lock<std::mutex> lock(m_value_mutex);

        if (pred())
        {
            return true;
        }

        return m_value_changed.wait(lock, pred);
    }

    void wait_for_any(const std::initializer_list<T>& values, const std::chrono::steady_clock::duration& duration) const
    {
        if (!try_wait_for_any(values, duration))
        {
            throw timeout_exception();
        }
    }

    void wait_for_any(const std::initializer_list<T>& values) const
    {
        if (!try_wait_for_any(values, 24_hours))
        {
            throw timeout_exception();
        }
    }

    void set_if_in(T if_state, T target_state)
    {
        {
            std::unique_lock<std::mutex> lock(m_value_mutex);
            if (m_value == if_state)
            {
                m_value = target_state;
            }
        }
        m_value_changed.notify_all();
    }

private:
    mutable std::mutex m_value_mutex;
    mutable std::condition_variable m_value_changed;
    std::atomic<T> m_value;
};

} // concurrency
} // cpp