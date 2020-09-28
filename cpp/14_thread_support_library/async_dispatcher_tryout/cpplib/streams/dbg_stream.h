// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <streambuf>
#include <string>
#include <cpplib/win32/win_api.h>
#include <cpplib/types/non_copyable.h>

namespace cpp
{
namespace streams
{

template <class Elem, class Tr = std::char_traits<Elem>, class Alloc = std::allocator<Elem> >
class basic_debugbuf : public std::basic_streambuf<Elem, Tr>
{
    using int_type = typename std::basic_streambuf<Elem, Tr>::int_type;
    using traits_type = typename std::basic_streambuf<Elem, Tr>::traits_type;

protected:
    int sync()
    {
        m_buf[m_index] = 0;
        m_index = 0;
        output(m_buf);
        return 0;
    }

    int_type overflow(int_type c)
    {
        if (c == traits_type::eof())
        {
            return 0;
        }

        m_buf[m_index] = traits_type::to_char_type(c);
        m_index++;

        if (c == '\n' || m_index == m_bufferSize - 1)
        {
            sync();
        }

        return c;
    }

private:
    static const int m_bufferSize = 256;
    __declspec(thread) static Elem m_buf[m_bufferSize];
    __declspec(thread) static int m_index;

    static void output(const char* msg)
    {
        OutputDebugStringA(msg);
    }

    static void output(const wchar_t* msg)
    {
        OutputDebugStringW(msg);
    }
};

template <class Elem, class Tr, class Alloc>
Elem basic_debugbuf<Elem, Tr, Alloc>::m_buf[m_bufferSize] = { '\0' };

template <class Elem, class Tr, class Alloc>
int basic_debugbuf<Elem, Tr, Alloc>::m_index = 0;

template <class Elem, class Tr = std::char_traits<Elem> >
class basic_dbgstream : public std::basic_ostream<Elem, Tr>
{
public:
    basic_dbgstream() : std::basic_ostream<Elem, Tr>(&buf)
    {
    }

private:
    basic_debugbuf<Elem, Tr> buf;
};

template <class Elem, class Tr = std::char_traits<Elem>, class Alloc = std::allocator<Elem> >
class basic_nullbuf : public std::basic_streambuf<Elem, Tr>
{
    using int_type = typename std::basic_streambuf<Elem, Tr>::int_type;

protected:
    int_type overflow(int_type c)
    {
        return c;
    }
};

template <class Elem, class Tr = std::char_traits<Elem> >
class basic_nullstream : public std::basic_ostream<Elem, Tr>
{
public:
    basic_nullstream() : std::basic_ostream<Elem, Tr>(&buf)
    {
    }

private:
    basic_nullbuf<Elem, Tr> buf;
};

} // streams

using nullstream = streams::basic_nullstream<char>;
using wnullstream = streams::basic_nullstream<wchar_t>;

using dbgstream = streams::basic_dbgstream<char>;
using wdbgstream = streams::basic_dbgstream<wchar_t>;

__declspec(selectany) nullstream cnull;
__declspec(selectany) wnullstream wcnull;

__declspec(selectany) dbgstream cdbg;
__declspec(selectany) wdbgstream wcdbg;

class ScopedDebugStream :
    public NonCopyable
{
public:
    explicit ScopedDebugStream(const std::string& msg, std::ostream& os = cdbg);
    ~ScopedDebugStream();

    void Leave();

private:
    std::string m_msg;
    std::ostream& m_os;
};

} // cpp