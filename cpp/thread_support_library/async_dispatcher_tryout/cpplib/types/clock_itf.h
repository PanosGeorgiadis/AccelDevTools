// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <chrono>
#include <cpplib/types/interface.h>

namespace cpp
{
namespace time

{
class SystemClockItf :
    public Interface
{
public:
    virtual std::chrono::system_clock::time_point now() = 0;
};

class SteadyClockItf :
    public Interface
{
public:
    virtual std::chrono::steady_clock::time_point now() = 0;
};

} // time
} // cpp