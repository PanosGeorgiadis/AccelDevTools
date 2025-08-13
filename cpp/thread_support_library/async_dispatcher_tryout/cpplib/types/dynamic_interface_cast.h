// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <typeinfo>

namespace cpp
{

// variant of dynamic_cast that will throw if target type is not supported.
template<typename To, typename From>
auto dynamic_interface_cast(From&& from)
{
    auto value = dynamic_cast<To>(from);
    
    if (value == nullptr)
    {
        throw std::bad_cast();
    }

    return value;
}

} // cpp
