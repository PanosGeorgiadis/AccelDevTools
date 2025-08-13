// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#include <ostream>

#pragma once

namespace cpp
{

namespace streams
{

class stream_bool
{
public:
    explicit stream_bool(bool value) :
        m_value{ value }
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const stream_bool& b)
    {
        return os << (b.m_value ? "true" : "false");
    }

    friend std::wostream& operator<<(std::wostream& os, const stream_bool& b)
    {
        return os << (b.m_value ? L"true" : L"false");
    }

private:
    bool m_value;
};

} // streams
} // cpp