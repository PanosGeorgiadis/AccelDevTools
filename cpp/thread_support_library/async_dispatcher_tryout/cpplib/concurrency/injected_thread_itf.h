// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <string>
#include <cpplib/preprocessor/nodiscard.h>

namespace cpp
{
namespace concurrency
{

//--------------------------------------------------------------------------------------------------------------------

struct requires_injected_thread_exception :
    public std::runtime_error
{
    explicit requires_injected_thread_exception(const std::string& text) :
        std::runtime_error(text.c_str())
    {
    }
};

struct requires_not_injected_thread_exception :
    public std::runtime_error
{
    explicit requires_not_injected_thread_exception(const std::string& text) :
        std::runtime_error(text.c_str())
    {
    }
};

//--------------------------------------------------------------------------------------------------------------------

struct InjectedThreadItf
{
    NO_DISCARD virtual const bool is_injected_thread() const = 0;
    virtual void injected_thread_required(const std::string&) const = 0;
    virtual void not_injected_thread_required(const std::string&) const = 0;

    virtual void notify(const std::function<void()> fn) const = 0;
    virtual void synchronize() const = 0;
};

//--------------------------------------------------------------------------------------------------------------------

struct NullInjectedThread :
    public InjectedThreadItf
{
    virtual const bool is_injected_thread() const override { return true;  }
    virtual void injected_thread_required(const std::string&) const override {}
    virtual void not_injected_thread_required(const std::string&) const override {}
    virtual void notify(const std::function<void()> fn) const override { fn(); }
    virtual void synchronize() const override {};

    static NullInjectedThread& instance()
    {
        static NullInjectedThread obj;
        return obj;
    }
};

} // concurrency
} // cpp
