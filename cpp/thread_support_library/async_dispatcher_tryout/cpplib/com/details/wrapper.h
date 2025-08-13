// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

// Value type trait for raw COM types:
//
// ComWrapper<int>::type -> int
// ComWrapper<BSTR>::type -> CComBSTR
// ComWrapper<Itf*>::type -> CComPtr<Itf>

#pragma once

#include <cassert>
#include <algorithm>
#include <atlsafe.h>
#include <type_traits>
#include <cpplib/com/traits.h>
#include <cpplib/com/checked_hresult.h>
#include <cpplib/link_cpplib.h>
#include <cpplib/types/uuid.h>

namespace cpp
{
namespace com
{

template <typename T, typename Enable = void>
struct ComWrapper
{
    using type = T;
};

template <typename Itf>
struct ComWrapper<Itf*, typename std::enable_if<is_com_interface<Itf>::value>::type>
{
	using type = ATL::CComPtr<Itf>;
};

template <>
struct ComWrapper<BSTR>
{
	using type = ATL::CComBSTR;
};

template <>
struct ComWrapper<VARIANT>
{
	using type = ATL::CComVariant;
};

class SafeArrayWrapper
{
public:
	SafeArrayWrapper();
	SafeArrayWrapper(SafeArrayWrapper& other);

	~SafeArrayWrapper();

	SafeArrayWrapper& operator=(SafeArrayWrapper other);

	void swap(SafeArrayWrapper& other);

	template <typename T>
	operator typename ATL::CComSafeArray<T>()
	{
		assert(m_psa);
		ATL::CComSafeArray<T> sa;
		CheckedHResult chr = sa.Attach(m_psa);
		m_psa = nullptr;
		return sa;
	}

	SAFEARRAY** operator&();
	SAFEARRAY* get() const;
	SAFEARRAY* detach();

private:
	SAFEARRAY* m_psa;
};

template <>
struct ComWrapper<SAFEARRAY*>
{
	using type = SafeArrayWrapper;
};

HRESULT ComRetVal(SafeArrayWrapper& sa, SAFEARRAY** ppValue);
HRESULT ComRetVal(SafeArrayWrapper&& sa, SAFEARRAY** ppValue);

} // com 
} // cpp
