// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{

class NonMoveable
{
public:
    NonMoveable(NonMoveable&&) = delete;

protected:
    NonMoveable() = default;
    virtual ~NonMoveable() = default;
};

} // cpp
