// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <iosfwd>
#include <functional>
#include <string>

#include <cpplib/com/atlcom_api.h>
#include <cpplib/link_cpplib.h>
#include <cpplib/com/checked_hresult.h>
#include <cpplib/com/traits.h>
#include <cpplib/traits/identity.h>
#include <cpplib/types/non_copyable.h>
#include <cpplib/types/uuid.h>
#include <cpplib/types/string_builder.h>

namespace cpp
{
namespace com
{

struct ComInitialization : NonCopyable
{
    static constexpr COINIT ApartmentThreaded = COINIT_APARTMENTTHREADED;
    static constexpr COINIT Multithreaded = COINIT_MULTITHREADED;

    explicit ComInitialization(COINIT coInit);
    ~ComInitialization();
};

template <typename COINIT CoInit>
struct ComInitializationEx : ComInitialization
{
    ComInitializationEx() :
        ComInitialization(CoInit)
    {
    }
};

template <typename Itf>
ATL::CComPtr<Itf> CreateComInstance(const CLSID& clsid, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
{
    ATL::CComPtr<Itf> pItf;
    CheckedHResult chr(pItf.CoCreateInstance(clsid, pUnkOuter, dwClsContext), stringbuilder() << "CoCreateInstance(" << uuid(clsid).str() << ")");
    return pItf;
}

template <typename Itf>
ATL::CComPtr<Itf> CreateComInstance(const std::string& progId, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
{
    ATL::CComPtr<Itf> pItf;
    CheckedHResult chr(pItf.CoCreateInstance(string_cast<ATL::CComBSTR>(progId), pUnkOuter, dwClsContext), stringbuilder() << "CoCreateInstance(\"" << progId << "\")");
    return pItf;
}

template <typename Itf>
ATL::CComPtr<Itf> CreateComInstance(const std::wstring& progId, IUnknown* pUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
{
    return CreateComInstance<Itf>(string_cast<std::string>(progId), pUnkOuter, dwClsContext);
}

template <typename Itf>
ATL::CComPtr<Itf> GetInterface(IUnknown* pIUnk)
{
    ATL::CComPtr<Itf> pItf;
    if (pIUnk != nullptr)
    {
        CheckedHResult chr = pIUnk->QueryInterface(&pItf);
    }
    return pItf;
}

#undef com_cast // ATL's com_cast doesn't work with com_cast<IUnkown>()
template <typename Itf>
ATL::CComPtr<Itf> com_cast(IUnknown* pIUnk)
{
    return GetInterface<Itf>(pIUnk);
}

bool IsSameComObject(IUnknown* pIUnk1, IUnknown* pIUnk2);
std::wstring ProgIDFromCLSID(const CLSID& clsid);
std::wstring TypeNameFromCLSID(const CLSID& clsid, USERCLASSTYPE userClassType = USERCLASSTYPE_FULL);

CLSID CLSIDFromProgID(const wchar_t* progId);
CLSID CLSIDFromProgID(const char* progId);
CLSID CLSIDFromProgID(const std::wstring& progId);
CLSID CLSIDFromProgID(const std::string& progId);

CLSID CLSIDFromString(const wchar_t* progId);
CLSID CLSIDFromString(const char* progId);
CLSID CLSIDFromString(const std::wstring& progId);
CLSID CLSIDFromString(const std::string& progId);

template <typename T>
HRESULT ComRetVal(typename traits::identity<T>::type value, T* retval)
{
    if (retval == nullptr) return E_POINTER;
    *retval = value;
    return S_OK;
}

template <typename Itf>
typename std::enable_if<is_com_interface<Itf>::value, HRESULT>::type ComRetVal(typename traits::identity<Itf*>::type pItf, Itf** retval)
{
    if (retval == nullptr) return E_POINTER;
    *retval = ATL::CComPtr<Itf>(pItf).Detach();
    return S_OK;
}

inline HRESULT ComRetVal(ATL::CComVariant value, VARIANT* retval)
{
    return value.Detach(retval);
}

inline HRESULT ComRetVal(bool value, VARIANT_BOOL* retval)
{
    if (retval == nullptr) return E_POINTER;
    *retval = value ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

inline HRESULT ComRetVal(SAFEARRAY* psa, SAFEARRAY** retval)
{
    if (retval == nullptr) return E_POINTER;
    return SafeArrayCopy(psa, retval);
}

inline HRESULT ComRetVal(ATL::CComBSTR value, BSTR* retval)
{
    if (retval == nullptr) return E_POINTER;
    
    *retval = value.Detach();
    return S_OK;
}

inline HRESULT ComRetVal(const std::string& value, BSTR* retval)
{
    if (retval == nullptr) return E_POINTER;

    *retval = ATL::CComBSTR(static_cast<int>(value.size()), value.c_str()).Detach();
    return S_OK;
}

inline HRESULT ComRetVal(const std::wstring& value, BSTR* retval)
{
    if (retval == nullptr) return E_POINTER;
    *retval = ATL::CComBSTR(static_cast<int>(value.size()), value.c_str()).Detach();
    return S_OK;
}

inline HRESULT ComRetVal(char* value, BSTR* retval)
{
    if ((value == nullptr) || (retval == nullptr)) return E_POINTER;
    *retval = ATL::CComBSTR(static_cast<int>(std::char_traits<char>::length(value)), value).Detach();
    return S_OK;
}

inline HRESULT ComRetVal(const char* value, BSTR* retval)
{
    if ((value == nullptr) || (retval == nullptr)) return E_POINTER;
    *retval = ATL::CComBSTR(static_cast<int>(std::char_traits<char>::length(value)), value).Detach();
    return S_OK;
}

inline HRESULT ComRetVal(wchar_t* value, BSTR* retval)
{
    if ((value == nullptr) || (retval == nullptr)) return E_POINTER;
    *retval = ATL::CComBSTR(static_cast<int>(std::char_traits<wchar_t>::length(value)), value).Detach();
    return S_OK;
}

inline HRESULT ComRetVal(const wchar_t* value, BSTR* retval)
{
    if ((value == nullptr) || (retval == nullptr)) return E_POINTER;
    *retval = ATL::CComBSTR(static_cast<int>(std::char_traits<wchar_t>::length(value)), value).Detach();
    return S_OK;
}

HRESULT ComRetValCall(const std::function<void()>& fn);

template <typename Fn, typename T>
HRESULT ComRetValCall(Fn&& fn, T* pRetVal)
{
    return ComRetValCall([&fn, pRetVal]() { CheckedHResult chr = ComRetVal(fn(), pRetVal); });
}

} // com 
} // cpp