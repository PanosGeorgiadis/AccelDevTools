// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{

class NonCreateable
{
public: 
    virtual ~NonCreateable() = default;

protected:
    NonCreateable() = default;
    NonCreateable(const NonCreateable&) = delete;
    NonCreateable& operator=(const NonCreateable&) = delete;
    NonCreateable(NonCreateable&&) = delete;
};

} // cpp


