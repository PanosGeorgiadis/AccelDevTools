// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

//TICS -OLC#009 "Avoid the use of so-called "magic numbers" in arbitrary expressions"

#pragma once
#include <iosfwd>

namespace cpp
{
namespace format
{

constexpr int base64_value(const char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }

    if (c >= 'a' && c <= 'z')
    {
        return 26 + c - 'a';
    }

    if (c >= '0' && c <= '9')
    {
        return 52 + c - '0';
    }

    if (c == '+')
    {
        return 62;
    }

    if (c == '/')
    {
        return 63;
    }

    return -1;
}

template <typename InIt, typename OutIt>
void base64_decode(InIt begin, InIt end, OutIt out)
{
    unsigned data = 0;
    int bits = 0;
    while (begin != end)
    {
        int v = base64_value(*begin++);
        if (v < 0)
        {
            continue;
        }

        data = (data << 6) | v;
        bits += 6;
        
        if (bits < 8)
        {
            continue;
        }

        bits -= 8;
        *out++ = static_cast<unsigned char>(data >> bits);
        data &= ~(0xFF << bits);
    }
}

class base64
{
public:
    base64(const void* data, size_t size) : 
        m_data{data}, 
        m_size{size}
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const base64& b)
    {
        static const char key[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        size_t n = b.m_size;
        const unsigned char* p = static_cast<const unsigned char*>(b.m_data);
        int chars = 0;
        while (n >= 3)
        {
            unsigned d = (p[0] << 16) | (p[1] << 8) | p[2];
            os << key[(d >> 18) & 0x3F] <<
                key[(d >> 12) & 0x3F] <<
                key[(d >> 6) & 0x3F] <<
                key[d & 0x3F];
            n -= 3;
            p += 3;
            chars += 4;
            if (chars % 76 == 0)
                os << '\n';
        }
        if (n == 2)
        {
            unsigned d = (p[0] << 16) | (p[1] << 8);
            os << key[(d >> 18) & 0x3F] <<
                key[(d >> 12) & 0x3F] <<
                key[(d >> 6) & 0x3F] <<
                '=';
        }
        else if (n == 1)
        {
            unsigned d = (p[0] << 16);
            os << key[(d >> 18) & 0x3F] <<
                key[(d >> 12) & 0x3F] <<
                "==";
        }

        return os;
    }

private:
    const void* m_data;
    size_t m_size;
};

} // format
} // cpp