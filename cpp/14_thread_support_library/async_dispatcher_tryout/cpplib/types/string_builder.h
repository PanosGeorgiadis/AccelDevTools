// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

/*
 * stringbuilder.h
 *
 * Example:
 *
 *  f(const std::string&);
 *  g(const std::wstring&);
 *
 *	f(stringbuilder() << "Hello world! " << 12);
 *	g(wstringbuilder() << L"Hello world! " << 12);
 */

#pragma once

#include <string>
#include <sstream>

namespace cpp
{

template <typename CharType, typename Traits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType> >
class basic_stringbuilder
{
public:
	using string_type = typename std::basic_string<CharType, Traits, Allocator>;

	template <typename T>
	basic_stringbuilder& operator<<(const T& t)
	{
		m_ss << t;
		return *this;
	}

	string_type str() const
	{
		return m_ss.str();
	}

	operator string_type() const
	{
		return str();
	}

private:
	std::basic_ostringstream<CharType, Traits, Allocator> m_ss;
};

using stringbuilder = basic_stringbuilder<char>;
using wstringbuilder = basic_stringbuilder<wchar_t> ;

} // cpp


