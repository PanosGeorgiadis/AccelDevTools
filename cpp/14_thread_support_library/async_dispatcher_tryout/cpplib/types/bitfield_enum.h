// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <type_traits>
#include <cstdint>

namespace cpp
{
namespace details
{

template<typename T>
struct is_bitfield_enum :
    public std::false_type
{
};

template<typename T>
inline constexpr bool is_bitfield_enum_v = is_bitfield_enum<T>::value;

} // details
} // cpp

template<typename T>
auto operator &(T lhs, T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T>
{
    static_assert(std::is_enum_v<T>);
    return static_cast<T> (static_cast<std::underlying_type_t<T>>(lhs) & static_cast<std::underlying_type_t<T>>(rhs));
}

template<typename T>
auto operator |(T lhs, T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T>
{
    static_assert(std::is_enum_v<T>);
    return static_cast<T> (static_cast<std::underlying_type_t<T>>(lhs) | static_cast<std::underlying_type_t<T>>(rhs));
}


template<typename T>
auto operator ^(T lhs, T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T>
{
    return static_cast<T> (static_cast<std::underlying_type_t<T>>(lhs) ^ static_cast<std::underlying_type_t<T>>(rhs));
}

template<typename T>
auto operator ~(T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T>
{
    return static_cast<T> (~static_cast<std::underlying_type_t<T>>(rhs));
}

template<typename T>
auto operator |=(T &lhs, T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T&>
{
    lhs = static_cast<T> (static_cast<std::underlying_type_t<T>>(lhs) | static_cast<std::underlying_type_t<T>>(rhs));

    return lhs;
}

template<typename T>
auto operator &=(T &lhs, T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T&>
{
    lhs = static_cast<T> (static_cast<std::underlying_type_t<T>>(lhs) & static_cast<std::underlying_type_t<T>>(rhs));
    return lhs;
}

template<typename T>
auto operator ^=(T &lhs, T rhs) -> typename std::enable_if_t<cpp::details::is_bitfield_enum_v<T>, T&>
{
    lhs = static_cast<T> (static_cast<std::underlying_type_t<T>>(lhs) ^ static_cast<std::underlying_type_t<T>>(rhs));
    return lhs;
}
