// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <type_traits>
#include <cpplib/link_cpplib.h>
#include <cpplib/win32/win_api.h> // GUID declaration
#include "details/details_uuid.h"

//TICS -OLC#009 "Avoid the use of so-called "magic numbers" in arbitrary expressions"

namespace cpp
{

struct uuid final
{
public:
	constexpr uuid() = default;
	~uuid() = default;
	uuid(const uuid&) = default;
	uuid& operator=(const uuid&) = default;

	explicit uuid(GUID guid);

	static const uuid null() noexcept;
	static const uuid new_uuid();

	bool operator==(const uuid& other) const noexcept;
	bool operator!=(const uuid& other) const noexcept;
	bool operator<(const uuid& other) const noexcept;
    bool operator>(const uuid& other) const noexcept;

	const std::string str() const;
	const std::wstring wstr() const;

	template<size_t N>
	void parse(const char(&str)[N]) noexcept
	{
		static_assert(false, "unrecognized uuid format");
	}

    // string has correct length so consider it to be a uuid
	template<>
	void parse<39>(const char(&str)[39]) noexcept
	{
		const char* p = str;
		p++;

		m_uuid.data1 = details::ParseHex<std::uint32_t>(p);
		p += 9; // skip -
		m_uuid.data2 = details::ParseHex<std::uint16_t>(p);
		p += 5; // skip - 
		m_uuid.data3 = details::ParseHex<std::uint16_t>(p);
		p += 5; // skip - 
		for (unsigned int i = 0; i < 8u; ++i)
		{
			m_uuid.data4[i] = details::ParseHex<std::uint8_t>(p);
			p += 2;
			if (i == 1) ++p; // skip -
		}
	};
    
private:
    template<typename T>
    auto to_string() const
    {
        std::basic_stringstream<T> ss;
        ss << std::setfill(details::uuid_string<T>::zero)
            << std::uppercase
            << std::hex
            << std::setw(8) << m_uuid.data1 << '-'
            << std::setw(4) << m_uuid.data2 << '-'
            << std::setw(4) << m_uuid.data3 << '-'
            << std::setw(2) << static_cast<short>(m_uuid.data4[0])
            << std::setw(2) << static_cast<short>(m_uuid.data4[1])
            << '-'
            << std::setw(2) << static_cast<short>(m_uuid.data4[2])
            << std::setw(2) << static_cast<short>(m_uuid.data4[3])
            << std::setw(2) << static_cast<short>(m_uuid.data4[4])
            << std::setw(2) << static_cast<short>(m_uuid.data4[5])
            << std::setw(2) << static_cast<short>(m_uuid.data4[6])
            << std::setw(2) << static_cast<short>(m_uuid.data4[7]);

        return ss.str();
    };

	details::uuid_data m_uuid = {};
};

template<size_t N>
constexpr uuid make_guid(const char(&data)[N]) noexcept
{
	uuid id;
	id.parse(data);
	return id;
}

// for now resort to macro to do compile time validation of string
#define StaticGuid(str) \
	make_guid(str); \
	static_assert(str[0]=='{', "uuid doesn't start with a '{'"); \
	static_assert(details::is_hex_str(str, 1, 8), "uuid : invalid hexadecimal data1"); \
	static_assert((str[9] == '-'), "uuid : character at offset 9 should be a '-'"); \
	static_assert(details::is_hex_str(str, 10, 4), "uuid : invalid hexadecimal data2"); \
	static_assert((str[14] == '-'), "uuid : character at offset 14 should be a '-'"); \
	static_assert(details::is_hex_str(str, 15, 4), "uuid : invalid hexadecimal data3"); \
	static_assert((str[19] == '-'), "uuid : character at offset 19 should be a '-'"); \
	static_assert(details::is_hex_str(str, 20, 4), "uuid : invalid hexadecimal data4[0..1]"); \
	static_assert((str[24] == '-'), "uuid : character at offset 24 should be a '-'"); \
	static_assert(details::is_hex_str(str, 25, 6), "uuid : invalid hexadecimal data4[2..7]"); \
	static_assert((str[37] == '}'), "uuid doesn't end with '}'");

} // cpp

namespace std
{
template<>
struct less<cpp::uuid>
{
	bool operator() (const cpp::uuid& lhs, const cpp::uuid& rhs) const
	{
		return lhs < rhs;
	}
};

} // std

