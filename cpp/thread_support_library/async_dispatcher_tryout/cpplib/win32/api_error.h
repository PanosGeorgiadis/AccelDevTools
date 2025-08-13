// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <string>

#include <cpplib/win32/win_api.h>
#include <cpplib/link_cpplib.h>

namespace cpp
{
namespace win32
{

//-----------------------------------------------------------------------------------------------------------------
//
// exception class to be thrown when a Win32 api call fails.
//

class ApiError :
	public std::runtime_error
{
public:
	ApiError() = delete;
	virtual ~ApiError() = default;
	ApiError(DWORD code, const std::string& message);

	const DWORD value() const;

private:
	DWORD m_code;
};

} // win32
} // cpp
