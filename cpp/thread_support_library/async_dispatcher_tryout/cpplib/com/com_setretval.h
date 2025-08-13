// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <type_traits>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/traits.h>
#include <cpplib/types/boolean_cast.h>
#include <cpplib/types/string_cast.h>

namespace cpp
{
namespace com
{

template<typename T>
inline HRESULT SetRetval(T* retval, const T& value)
{
    if (retval == nullptr) return E_POINTER;
    *retval = value;
    return S_OK;
}

template<typename Itf>
inline HRESULT SetRetval(Itf** retval, const ATL::CComPtr<Itf>& value)
{
    if (retval == nullptr) return E_POINTER;
    *retval = ATL::CComPtr<Itf>(value).Detach();
    return S_OK;
}

inline HRESULT SetRetval(BSTR* retval, const std::string& value)
{
    if (retval == nullptr) return E_POINTER;
    *retval = ATL::CComBSTR(string_cast<std::wstring>(value).c_str()).Detach();
    return S_OK;
}

inline HRESULT SetRetval(BSTR* retval, const std::wstring& value)
{
    if (retval == nullptr) return E_POINTER;
    *retval = ATL::CComBSTR(value.c_str()).Detach();
    return S_OK;
}

inline HRESULT SetRetval(VARIANT* retval, const ATL::CComVariant& value)
{
    if (retval == nullptr) return E_POINTER;
    return ATL::CComVariant(value).Detach(retval);
}

inline HRESULT SetRetval(VARIANT_BOOL* retval, bool value)
{
    if (retval == nullptr) return E_POINTER;
    *retval = boolean_cast<VARIANT_BOOL>(value);
    return S_OK;
}

inline HRESULT SetRetval(SAFEARRAY** retval, SAFEARRAY* value)
{
    if (retval == nullptr) return E_POINTER;
    return ::SafeArrayCopy(value, retval);
}

} // com
} // cpp
