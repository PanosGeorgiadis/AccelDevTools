// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <string>

namespace cpp
{

class invalid_state_exception :
    public std::exception
{
public:
    invalid_state_exception() :
        std::exception()
    {
    }

    invalid_state_exception(const std::string& str) :
        std::exception(str.c_str())
    {
    }
};

} // cpp