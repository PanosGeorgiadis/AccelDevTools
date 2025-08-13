// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{

class Interface
{
public:
    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;
    Interface(Interface&&) = delete;
    virtual ~Interface() = default;

protected:
    Interface() = default;
    
};

} // cpp
