// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <cpplib/types/interface.h>

namespace cpp
{

class EventConnectionItf :
    public Interface
{
public:
    virtual void disconnect() = 0;
};

} // cpp
