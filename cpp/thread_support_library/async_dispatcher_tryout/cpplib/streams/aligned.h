// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <sstream>

namespace cpp
{
namespace streams
{

template <typename CharType, typename CharTraits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType> >
class basic_left_aligned
{
public:
    using ostream_type = std::basic_ostream<CharType, CharTraits>;
    using string_type = typename std::basic_string<CharType, CharTraits, Allocator>;
    using size_type = typename string_type::size_type;

    explicit basic_left_aligned(size_type width) :
        m_width{ width }
    {
    }

    template <typename T>
    basic_left_aligned& operator<<(const T& t)
    {
        m_ss << t;
        return *this;
    }

    friend ostream_type& operator<<(ostream_type& os, const basic_left_aligned& la)
    {
        os << la.m_ss.str();
        size_type width = la.m_ss.str().size();
        if (width < la.m_width)
            os << string_type(la.m_width - width, ' ');
        return os;
    }

private:
    std::basic_ostringstream<CharType, CharTraits, Allocator> m_ss;
    size_type m_width;
};

using left_aligned = basic_left_aligned<char>;
using wleft_aligned = basic_left_aligned<wchar_t>;

template <typename CharType, typename CharTraits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType> >
class basic_center_aligned
{
public:
    using ostream_type = std::basic_ostream<CharType, CharTraits>;
    using string_type = typename std::basic_string<CharType, CharTraits, Allocator>;
    using size_type = typename string_type::size_type;

    explicit basic_center_aligned(size_type width) :
        m_width{ width }
    {
    }

    template <typename T>
    basic_center_aligned& operator<<(const T& t)
    {
        m_ss << t;
        return *this;
    }

    friend ostream_type& operator<<(ostream_type& os, const basic_center_aligned& ca)
    {
        size_type width = ca.m_ss.str().size();
        size_type la = 0;
        size_type ra = 0;
        if (width < ca.m_width)
        {
            la = (ca.m_width - width) / 2;
            ra = (ca.m_width - width) - la;
        }

        return os << string_type(la, ' ') << ca.m_ss.str() << string_type(ra, ' ');
    }

private:
    std::basic_ostringstream<CharType, CharTraits, Allocator> m_ss;
    size_type m_width;
};

using center_aligned = basic_center_aligned<char>;
using wcenter_aligned = basic_center_aligned<wchar_t>;

template <typename CharType, typename CharTraits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType> >
class basic_right_aligned
{
public:
    using ostream_type = std::basic_ostream<CharType, CharTraits>;
    using string_type = typename std::basic_string<CharType, CharTraits, Allocator>;
    using size_type = typename string_type::size_type;

    explicit basic_right_aligned(size_type width) :
        m_width{ width }
    {
    }

    template <typename T>
    basic_right_aligned& operator<<(const T& t)
    {
        m_ss << t;
        return *this;
    }

    friend ostream_type& operator<<(ostream_type& os, const basic_right_aligned& ra)
    {
        size_type width = ra.m_ss.str().size();
        if (width < ra.m_width)
            os << string_type(ra.m_width - width, ' ');
        return os << ra.m_ss.str();
    }

private:
    std::basic_ostringstream<CharType, CharTraits, Allocator> m_ss;
    size_type m_width;
};

using right_aligned = basic_right_aligned<char>;
using wright_aligned = basic_right_aligned<wchar_t>;

} // streams
} // cpp