// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#include <iosfwd>

#pragma once

namespace cpp
{
namespace streams
{

class stream_binary
{
public:
    explicit stream_binary(unsigned value) :
        m_value{ value }
    {
    }

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const stream_binary& b)
    {
        unsigned value = b.m_value;
        unsigned bit = 0;

        do
        {
            ++bit;
            value >>= 1;
        } while (value > 0);

        while (bit > 0)
        {
            --bit;
            os.put((b.m_value & (1 << bit)) ? '1' : '0');
        }

        return os;
    }

private:
    unsigned m_value;
};

} // streams
} // cpp