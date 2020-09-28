// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <cpplib/units/time.h>

namespace cpp
{
// using std::chrono::duration::max() as infinite will overflow some std:: api's so we need
// to use something a bit smaller that's effectively still too long to wait for.
constexpr auto max_timeout = 360_days;

class timeout_exception :
	public std::exception
{
};

} // cpp
