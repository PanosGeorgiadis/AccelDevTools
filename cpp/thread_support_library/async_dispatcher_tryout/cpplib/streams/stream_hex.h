// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#include <iosfwd>

#pragma once

namespace cpp
{
namespace streams
{

class stream_hex
{
public:
    explicit stream_hex(int value) :
        m_value{ value }
    {
    }

    template <typename CharT, typename Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const stream_hex& h)
    {
        std::ios_base::fmtflags flags = os.setf(std::ios_base::hex, std::ios_base::basefield);
        os << h.m_value;
        os.setf(flags, std::ios_base::basefield);
        return os;
    }

private:
    int m_value;
};

} // streams
} // cpp