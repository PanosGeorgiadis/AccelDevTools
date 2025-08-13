// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <type_traits>

namespace cpp
{
namespace traits
{
// for_all, to be used to check variadic template arguments in 
// static asserts
// example:
//  
// static_assert(for_all<std::is_same<T, Args>::value...>::value, "at least one argument not of type T");
//
template <bool... all> struct for_all
{
};

template <bool... tail>
struct for_all<true, tail...> : for_all < tail... >
{
};

template <bool... tail>
struct for_all<false, tail...> : std::false_type
{
};

template <> struct
for_all<> : std::true_type
{
};

} // traits
} // cpp