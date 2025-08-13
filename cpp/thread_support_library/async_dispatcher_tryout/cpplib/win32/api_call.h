// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/win32/details/checked_api_error.h>

namespace cpp
{
namespace win32
{

template<typename Fn, typename... Args>
void api_call(Fn&& fn, Args&&... args)
{
    auto lambda = [&fn, &args...]
    {
        return fn(std::forward<Args>(args)...);
    };

    details::checked_api_eror err = lambda();
}

} // win32
} // cpp

