// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

namespace cpp
{

class not_implemented_exception :
	public std::exception
{
public:
	not_implemented_exception(const std::string& msg) :
		std::exception(msg.c_str())
	{
	}

	not_implemented_exception(const std::string& func, const std::string& msg) :
		std::exception(format(func,msg).c_str())
	{
	}

private:
	static std::string format(const std::string& func, const std::string& msg) noexcept
	{
		try
		{
		std::stringstream stream;
		stream << func << " : " << msg;
		return stream.str();
		}
		catch (const std::exception&)
		{
			return msg;
		}
	}
};

} // cpp
