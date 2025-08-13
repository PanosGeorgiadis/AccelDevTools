// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <wtypes.h>
#include <cpplib/link_cpplib.h>

namespace cpp
{
namespace win32
{
namespace details
{

class checked_api_eror
{
public:
	checked_api_eror() = delete;
	virtual ~checked_api_eror() = default;
	checked_api_eror(const checked_api_eror&) = delete;
	checked_api_eror& operator=(const checked_api_eror&) = delete;
	checked_api_eror(BOOL retval);  //TICS !INT#001 implicit by design to be able to write one-liner checks!
	void operator=(const BOOL retval);

	static void throw_error();

private:
	void check(BOOL retval);
};

} // details
} // win32
} // details
