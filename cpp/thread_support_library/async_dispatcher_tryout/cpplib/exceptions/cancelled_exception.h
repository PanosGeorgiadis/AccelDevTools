// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <string>

namespace cpp
{

class cancelled_exception:
    public std::exception
{
public:
    cancelled_exception() :
        std::exception()
    {
    }

    cancelled_exception(const std::string& str) :
        std::exception(str.c_str())
    {
    }
};

} // cpp