// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <functional>

namespace cpp
{
namespace traits
{
namespace details
{
template<typename T>
struct memfun_type
{
    using type = void;
};

template<typename RetvalType, typename ClassType, typename... Args>
struct memfun_type<RetvalType(ClassType::*)(Args...) const>
{
    using type = typename std::function<RetvalType(Args...)>;
};

} // details

template<typename Fn>
typename details::memfun_type<decltype(&Fn::operator())>::type 
make_std_function(Fn const& fn)
{
    return fn;
}

} // traits
} // cpp
