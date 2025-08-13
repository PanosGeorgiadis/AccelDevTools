// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <chrono>

constexpr std::chrono::nanoseconds operator "" _ns(unsigned long long value)
{
    return std::chrono::nanoseconds(value);
}

constexpr std::chrono::milliseconds operator "" _ms(unsigned long long value)
{
    return std::chrono::milliseconds(value);
}

constexpr std::chrono::seconds operator "" _s(unsigned long long value)
{
    return std::chrono::seconds(value);
}

constexpr std::chrono::hours operator "" _hours(unsigned long long value)
{
    return std::chrono::hours(value);
}

constexpr std::chrono::hours operator "" _days(unsigned long long value)
{
    return std::chrono::hours(24 * value);
}

constexpr std::chrono::hours operator "" _weeks(unsigned long long value)
{
    return std::chrono::hours(7 * 24 * value);
}

//-----------------------------------------------------------------------------------------------------------------------------------------

