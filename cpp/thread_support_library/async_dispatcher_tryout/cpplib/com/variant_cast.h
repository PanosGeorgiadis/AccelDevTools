// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

// explicit type conversion from VARIANT:
// int i = variant_cast<int>(v);
// CComBSTR s = variant_cast<CComBSTR>(v);

#pragma once

#include <type_traits>
#include <string>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/traits.h>
#include <cpplib/com/safe_array.h>
#include <cpplib/com/details/wrapper.h>

//-----------------------------------------------------------------------------------------------------------------------------------------
// ATL's atlcomcli.h misses the signed char specialization for CVarTypeInfo:
//template <>

namespace ATL
{

template<>
class CVarTypeInfo<signed char>
{
public:
    static const VARTYPE VT = VT_I1;
    static char VARIANT::* const pmField;
};

__declspec(selectany) char VARIANT::* const CVarTypeInfo<signed char>::pmField = &VARIANT::cVal;

} // ATL

//-----------------------------------------------------------------------------------------------------------------------------------------

namespace cpp
{
namespace com
{

template <typename T, typename Enable = void>
struct VarTypeInfo : 
    public ATL::CVarTypeInfo<T>
{
    static T Get(const VARIANT& v)
    {
        return v.*(ATL::CVarTypeInfo<T>::pmField);
    }
};

template <>
struct VarTypeInfo<ATL::CComBSTR> : public VarTypeInfo<BSTR>
{
};

template <>
struct VarTypeInfo<std::wstring> : public VarTypeInfo<BSTR>
{
};

template <typename Itf>
struct VarTypeInfo<ATL::CComPtr<Itf>, typename std::enable_if<!is_dispatch_interface<Itf>::value>::type> : public VarTypeInfo<IUnknown*>
{
    static ATL::CComPtr<Itf> Get(const VARIANT& v)
    {
        return GetInterface<Itf>(v.punkVal);
    }
};

template <typename Itf>
struct VarTypeInfo<ATL::CComPtr<Itf>, typename std::enable_if<is_dispatch_interface<Itf>::value>::type> : public VarTypeInfo<IDispatch*>
{
    static ATL::CComPtr<Itf> Get(const VARIANT& v)
    {
        return GetInterface<Itf>(v.pdispVal);
    }
};

template <typename Itf>
struct VarTypeInfo<Itf*, typename std::enable_if<is_com_interface<Itf>::value>::type> : public VarTypeInfo<ATL::CComPtr<Itf>>
{
};

template <typename T>
struct VarTypeInfo<ATL::CComSafeArray<T>>
{
    static const VARTYPE VT = VT_ARRAY | VarTypeInfo<T>::VT;

    static ATL::CComSafeArray<T> Get(const VARIANT& v)
    {
        return ATL::CComSafeArray<T>(v.parray);
    }
};

template <>
struct VarTypeInfo<ATL::CComSafeArray<VARIANT>>
{
    static const VARTYPE VT = VT_ARRAY | VT_VARIANT;

    static ATL::CComSafeArray<VARIANT> Get(const VARIANT& v)
    {
        return ATL::CComSafeArray<VARIANT>(v.parray);
    }
};

template <typename T, unsigned N>
struct VarTypeInfo<SafeArray<T, N>>
{
    static const VARTYPE VT = VT_ARRAY | VarTypeInfo<T>::VT;

    static SafeArrayRef<T, N> Get(const VARIANT& v)
    {
        return SafeArrayRef<T, N>(v.parray);
    }
};

template <unsigned N>
struct VarTypeInfo<SafeArray<VARIANT, N>>
{
    static const VARTYPE VT = VT_ARRAY | VT_VARIANT;

    static SafeArrayRef<VARIANT, N> Get(const VARIANT& v)
    {
        return SafeArrayRef<VARIANT, N>(v.parray);
    }
};

template <typename T>
typename ComWrapper<T>::type variant_cast(const VARIANT& v)
{
    if (v.vt == VarTypeInfo<T>::VT)
    {
        return VarTypeInfo<T>::Get(v);
    }

    ATL::CComVariant v2;
    CheckedHResult chr = VariantChangeType(&v2, const_cast<VARIANT*>(&v), 0, VarTypeInfo<T>::VT);
    return VarTypeInfo<T>::Get(v2);
}

} // com 
} // cpp

