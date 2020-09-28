// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

// Streaming support for:
// - wide character strings into std::ostream objects.
// - nullptr as "nullptr"


#pragma once

#include <iostream>

namespace cpp
{

inline std::ostream& operator<<(std::ostream& os, const wchar_t* s)
{
    while (*s != 0)
    {
        os << static_cast<char>(*s);
        ++s;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const std::wstring& s)
{
    for (auto it = s.begin(), end = s.end(); it != end; ++it)
        os << static_cast<char>(*it);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const std::nullptr_t&)
{
    return os << "nullptr";
}

} // cpp