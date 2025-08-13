// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{
namespace details
{

class guard_base
{
public:
    guard_base() :
        m_released{ false }
    {
    }

    guard_base(const guard_base& rhs) :
        m_released(rhs.m_released)
    {
        rhs.release();
    }

    virtual ~guard_base() = default;
    
    guard_base& operator=(const guard_base& rhs) 
    {
        m_released = rhs.m_released;
        rhs.release();
    }

    guard_base(guard_base&& rhs) = delete; 


    void release() const
    {
        m_released = true;
    }

    const bool released() const noexcept
    {
        return m_released;
    }

private:
    mutable bool m_released;
};


template <class F>
class guard_impl : public guard_base
{
public:
    explicit guard_impl(const F& x) :
        m_action{ x }
    {
    }

    virtual ~guard_impl()
    {
        if (released())
        {
            return;
        }

        // Catch any exceptions and continue during guard clean up
        try
        {
            m_action();
        }
        catch (...)
        {
        }
    }

private:
    F m_action;
};

} // details

template <class F>
details::guard_impl<F> make_guard(F f)
{
    return details::guard_impl<F>(f);
}

using scope_guard = const details::guard_base&;

} // cpp

#define CPP_SCOPE_GUARD_CAT1(a, b) a##b
#define CPP_SCOPE_GUARD_CAT(a, b) CPP_SCOPE_GUARD_CAT1(a, b)
#define CPP_SCOPE_GUARD_VARIABLE(prefix) CPP_SCOPE_GUARD_CAT(prefix,__LINE__)

// SCOPE_GUARD() adds a bogus guard__xxx statement to supress false compiler warning "Unused variable guard_xxx" on VC2010:
#define SCOPE_GUARD(f) ::cpp::scope_guard CPP_SCOPE_GUARD_VARIABLE(guard_) = ::cpp::make_guard(f); CPP_SCOPE_GUARD_VARIABLE(guard_)
