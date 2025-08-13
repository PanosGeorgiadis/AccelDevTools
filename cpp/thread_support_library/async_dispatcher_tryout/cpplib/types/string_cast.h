// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

/*
 * string_cast: string type conversions for all common Win32 and C++ string types.
 *
 * Example:
 *
 * void f(BSTR bstr)
 * {
 *     std::string s = string_cast<std::string>(bstr);
 * }
 *
 * Supported types:
 *                   From:
 * To:           char*   const char*   wchar_t*   const wchar_t*   BSTR   _bstr_t   CComBSTR
 * std::string    v           v           v            v            v        v         v
 * std::wstring   v           v           v            v            v        v         v
 * _bstr_t        v           v           v            v            v        v         v
 * CComBSTR       v           v           v            v            v        v         v
 * BSTR           v           v           v            v            v        v         v
 */

#include <string>
#include <cpplib/win32/win_api.h>
#include <cpplib/com/atlcom_api.h>

 // todo move some stuff to details

namespace cpp
{

template <typename Dst, typename Src>
struct string_cast_helper
{
    using result_type = Dst;

    static Dst cast(const Src& src)
    {
        return static_cast<Dst>(src);
    }
};

template <typename Dst, typename Src>
typename string_cast_helper<Dst, Src>::result_type string_cast(const Src& src)
{
    return string_cast_helper<Dst, Src>::cast(src);
}

template <typename Src>
struct string_cast_helper<std::string, Src>
{
    using result_type = std::string;

    template <typename T>
    static std::string cast(const T& src)
    {
        return static_cast<std::string>(src);
    }

    static std::string cast(const std::wstring& src)
    {
        return std::string(src.begin(), src.end());
    }

    static std::string cast(const __wchar_t* src)
    {
        // NULL BSTR's may represent empty strings, we'll allow that here:
        return src ? std::string(src, src + wcslen(reinterpret_cast<const wchar_t*>(src))) : std::string();
    }

    static std::string cast(__wchar_t* src)
    {
        return cast(static_cast<const wchar_t*>(src));
    }

    static std::string cast(const unsigned short* src)
    {
        // NULL BSTR's may represent empty strings, we'll allow that here:
        return cast(reinterpret_cast<const __wchar_t*>(src));
    }

    static std::string cast(unsigned short* src)
    {
        return cast(static_cast<const unsigned short*>(src));
    }

    static std::string cast(const _bstr_t& src)
    {
        const char* p = src;
        return std::string(p, p + src.length());
    }

    static std::string cast(const ATL::CComBSTR& src)
    {
        return std::string(src.m_str, src.m_str + src.Length());
    }
};

template <typename Src>
struct string_cast_helper<std::wstring, Src>
{
    using result_type = std::wstring;

    template <typename T>
    static std::wstring cast(const T& src)
    {
        return static_cast<std::wstring>(src);
    }

    static std::wstring cast(const std::string& src)
    {
        return std::wstring(src.begin(), src.end());
    }

    static std::wstring cast(const char* src)
    {
        return std::wstring(src, src + strlen(src));
    }

    static std::wstring cast(char* src)
    {
        return std::wstring(src, src + strlen(src));
    }

    static std::wstring cast(const __wchar_t* src)
    {
        return src ? static_cast<std::wstring>(src) : std::wstring();
    }

    static std::wstring cast(__wchar_t* src)
    {
        return cast(static_cast<const wchar_t*>(src));
    }

    static std::wstring cast(const unsigned short* src)
    {
        // NULL BSTR's may represent empty strings, we'll allow that here:
        return cast(reinterpret_cast<const __wchar_t*>(src));
    }

    static std::wstring cast(unsigned short* src)
    {
        return cast(static_cast<const unsigned short*>(src));
    }

    static std::wstring cast(const _bstr_t& src)
    {
        const char* p = src;
        return std::wstring(p, p + src.length());
    }

    static std::wstring cast(const ATL::CComBSTR& src)
    {
        return src.m_str ? static_cast<std::wstring>(src.m_str) : std::wstring();
    }
};

template <typename Src>
struct string_cast_helper<ATL::CComBSTR, Src>
{
    using result_type = ATL::CComBSTR;

    template <typename T>
    static ATL::CComBSTR cast(const T& src)
    {
        return static_cast<ATL::CComBSTR>(src);
    }

    static ATL::CComBSTR cast(const std::string& src)
    {
        return ATL::CComBSTR(int(src.size()), src.c_str());
    }

    static ATL::CComBSTR cast(const std::wstring& src)
    {
        return ATL::CComBSTR(int(src.size()), src.c_str());
    }

    static ATL::CComBSTR cast(const _bstr_t& src)
    {
        return ATL::CComBSTR(static_cast<BSTR>(src));
    }

    static ATL::CComBSTR cast(const __wchar_t* src)
    {
        return ATL::CComBSTR(reinterpret_cast<const wchar_t*>(src));
    }

    static ATL::CComBSTR cast(__wchar_t* src)
    {
        return cast(static_cast<const __wchar_t*>(src));
    }

    static ATL::CComBSTR cast(const unsigned short* src)
    {
        return ATL::CComBSTR(reinterpret_cast<const wchar_t*>(src));
    }

    static ATL::CComBSTR cast(unsigned short* src)
    {
        return cast(static_cast<const unsigned short*>(src));
    }
};

template <typename Src>
struct string_cast_helper<BSTR, Src> : string_cast_helper<ATL::CComBSTR, Src>
{
};

template <typename Src>
struct string_cast_helper<_bstr_t, Src>
{
    using result_type = _bstr_t;

    template <typename T>
    static _bstr_t cast(const T& src)
    {
        return static_cast<_bstr_t>(src);
    }

    static _bstr_t cast(const std::string& src)
    {
        return _bstr_t(src.c_str());
    }

    static _bstr_t cast(const std::wstring& src)
    {
        return _bstr_t(src.c_str());
    }

    static _bstr_t cast(const __wchar_t* src)
    {
        return _bstr_t(reinterpret_cast<const wchar_t*>(src));
    }

    static _bstr_t cast(__wchar_t* src)
    {
        return cast(static_cast<const __wchar_t*>(src));
    }

    static _bstr_t cast(const unsigned short* src)
    {
        return _bstr_t(reinterpret_cast<const wchar_t*>(src));
    }

    static _bstr_t cast(unsigned short* src)
    {
        return cast(static_cast<const unsigned short*>(src));
    }
};

} // cpp

