// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#include <cpplib/com/com_base.h>

namespace cpp
{
namespace com
{

class RunningObjectTableRegistration
{
public:
	RunningObjectTableRegistration(IUnknown* pIUnk, const CLSID& clsid, DWORD dwFlags = ACTIVEOBJECT_STRONG);
	~RunningObjectTableRegistration();

private:
	DWORD m_dwRegister;
};

template <typename Itf>
ATL::CComPtr<Itf> GetActiveObject(const CLSID& clsid)
{
	ATL::CComPtr<IUnknown> pIUnk;
	CheckedHResult chr = ::GetActiveObject(clsid, nullptr, &pIUnk);
	return GetInterface<Itf>(pIUnk);
}

template <typename Itf>
ATL::CComPtr<Itf> GetActiveObject(const wchar_t* progId)
{
	return GetActiveObject<Itf>(CLSIDFromProgID(progId));
}

template <typename Itf>
ATL::CComPtr<Itf> GetActiveObject(const char* progId)
{
	return GetActiveObject<Itf>(CLSIDFromProgID(progId));
}

template <typename Itf>
ATL::CComPtr<Itf> GetActiveObject(const std::wstring& progId)
{
	return GetActiveObject<Itf>(progId.c_str());
}

template <typename Itf>
ATL::CComPtr<Itf> GetActiveObject(const std::string& progId)
{
	return GetActiveObject<Itf>(progId.c_str());
}

template <typename Itf>
ATL::CComPtr<Itf> QueryActiveObject(const CLSID& clsid)
{
	ATL::CComPtr<IUnknown> pIUnk;
	HRESULT ignored = ::GetActiveObject(clsid, nullptr, &pIUnk);
	static_cast<void>(ignored);
	return GetInterface<Itf>(pIUnk);
}

template <typename Itf>
ATL::CComPtr<Itf> QueryActiveObject(const wchar_t* progId)
{
	return QueryActiveObject<Itf>(CLSIDFromProgID(progId));
}

template <typename Itf>
ATL::CComPtr<Itf> QueryActiveObject(const char* progId)
{
	return QueryActiveObject<Itf>(CLSIDFromProgID(progId));
}

template <typename Itf>
ATL::CComPtr<Itf> QueryActiveObject(const std::wstring& progId)
{
	return QueryActiveObject<Itf>(progId.c_str());
}

template <typename Itf>
ATL::CComPtr<Itf> QueryActiveObject(const std::string& progId)
{
	return QueryActiveObject<Itf>(progId.c_str());
}

} // com 
} // cpp