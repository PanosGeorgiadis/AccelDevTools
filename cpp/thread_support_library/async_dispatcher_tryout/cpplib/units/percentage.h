// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <cpplib/link_cpplib.h>

class Percentage final
{
public:
    Percentage() = delete;
    ~Percentage() = default;
    Percentage(const Percentage&) = default;
    Percentage& operator=(const Percentage&) = default;
    Percentage(Percentage&&) = default;

    explicit Percentage(double value) :
        m_value{value}
    {
    }

    operator double() const
    {
        return m_value;
    }

    // TODO clipping 0-100
    // TODO make percentage behave like a double

private:
    double m_value;
};

Percentage operator "" _percent(long double value);

