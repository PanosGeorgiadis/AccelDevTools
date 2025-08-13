// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <string>

#include <cpplib/link_cpplib.h>
#include <cpplib/win32/win_api.h>

namespace cpp
{
namespace com
{

//-----------------------------------------------------------------------------------------------------------------
//
// exception class to be thrown when a com call fails.
//

class com_exception :
	public std::runtime_error
{
public:
	com_exception() = delete;
	virtual ~com_exception() = default;
	explicit com_exception(const HRESULT hr);
	com_exception(const HRESULT hr, const std::string& message);

	HRESULT GetError() const noexcept;

private:
	HRESULT m_hr;
};

} // com 
} // cpp
