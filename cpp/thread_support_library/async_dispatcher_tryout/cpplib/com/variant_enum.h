// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

// Example use:
//
//	STDMETHODIMP CMyCollection::get__NewEnum(IUnknown** ppIUnk)
//	{
//		using EnumImpl = ATL::CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, CopyToVariant, MyVector>;
//	
//		return CreateStlEnumerator<EnumImpl>(ppIUnk, static_cast<Com::IMyCollection*>(this), m_vector);
//	}

#pragma once

#include <string>
#include <cpplib/com/atlcom_api.h>

namespace cpp
{
namespace com
{

class CopyToVariant
{
public:
    template <typename T>
    static HRESULT copy(VARIANT* pTo, const T* pFrom)
    {
        return ATL::CComVariant(*pFrom).Detach(pTo);
    }

    static HRESULT copy(VARIANT* pTo, const std::string* pFrom)
    {
        return ATL::CComVariant(pFrom->c_str()).Detach(pTo);
    }

    static HRESULT copy(VARIANT* pTo, const std::wstring* pFrom)
    {
        return ATL::CComVariant(pFrom->c_str()).Detach(pTo);
    }

    static void init(VARIANT* p)
    {
        VariantInit(p);
    }

    static void destroy(VARIANT* p)
    {
        VariantClear(p);
    }
};

template <class EnumType, class CollType>
HRESULT CreateEnumerator(CollType& collection, IUnknown* pIUnkForRelease, IUnknown** ppIEnum)
{
    ATL::CComObject<EnumType>* pEnum;
    HRESULT hr = ATL::CComObject<EnumType>::CreateInstance(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }

    pEnum->AddRef();
    hr = pEnum->Init(pIUnkForRelease, collection);
    if (SUCCEEDED(hr))
    {
        hr = pEnum->QueryInterface(ppIEnum);
    }
    pEnum->Release();

    return hr;
}

template <typename CollType>
HRESULT CreateVariantEnum(CollType& collection, IUnknown* pIUnkForRelease, IUnknown** ppIEnum)
{
    using Enum = ATL::CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, CopyToVariant, CollType>;
    return CreateEnumerator<Enum>(collection, pIUnkForRelease, ppIEnum);
}

} // com 
} // cpp


