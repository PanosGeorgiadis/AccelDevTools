// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <cstdint>

//TICS -OLC#009 "Avoid the use of so-called "magic numbers" in arbitrary expressions"

namespace cpp
{
namespace details
{

constexpr const bool is_hex(const char c) noexcept
{
    return (('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'f')) || (('A' <= c) && (c <= 'F'));
}

constexpr const bool is_hex_str(const char* p0, size_t offset, size_t len) noexcept
{
    bool is_valid{ true };
    auto p = &p0[offset];
    size_t i{ 0 };

    while (is_valid && (i < len))
    {
        is_valid = (is_valid && is_hex(*p));
        p++;
        i++;
    }
    return is_valid;
}

template<typename R>
constexpr const R ParseHexDigit(const char c) noexcept
{
    return
        (('0' <= c) && (c <= '9')) ? c - '0' :
        (('a' <= c) && (c <= 'f')) ? c - 'a' + 10 :
        (('A' <= c) && (c <= 'F')) ? c - 'A' + 10 :
        0;
}

template<typename R>
constexpr const R ParseHex(const char* str) noexcept
{
    static_assert(std::is_integral_v<R>, "R must be an integral type");
    const size_t size = 2 * sizeof(R);

    R value = 0;
    for (unsigned int i = 0u; i < size; ++i)
    {
        value = value << 4u;
        value |= ParseHexDigit<R>(str[i]);
    }

    return value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// avoid code duplication between char and wchar_t variants of conversion to string

// 0 is different for char '0' and wchar L'0'
template<typename T> struct uuid_string {};

template<> struct uuid_string<char>
{
    static constexpr char zero = '0';
};

template<> struct uuid_string<wchar_t>
{
    static constexpr wchar_t zero = L'0';
};


#pragma pack(push)
#pragma pack(1)
struct uuid_data
{
    std::uint32_t data1;
    std::uint16_t data2;
    std::uint16_t data3;
    std::uint8_t data4[8];
};
#pragma pack(pop)

} // details
} // cpp