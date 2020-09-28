// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/com/atlcom_api.h>
#include <cpplib/traits/identity.h>

namespace cpp
{
namespace com
{
namespace details
{

template <typename T, typename IEvents, typename CDV = ATL::CComDynamicUnkArray>
class IUnkConnectionPointImpl : public ATL::IConnectionPointImpl<T, &__uuidof(IEvents), CDV>
{
public:
    IUnkConnectionPointImpl() = default;
    virtual ~IUnkConnectionPointImpl() = default;

protected:
    template <typename Fn, typename... Args>
    void Fire(Fn func, Args&&... args)
    {
        T* pT = static_cast<T*>(this);
        int nConnections = this->m_vec.GetSize();
        for (int nConnectionIndex = 0; nConnectionIndex < nConnections; ++nConnectionIndex)
        {
            pT->Lock();
            ATL::CComPtr<IUnknown> sp = this->m_vec.GetAt(nConnectionIndex);
            pT->Unlock();
            if (IEvents* pIEvents = static_cast<IEvents*>(sp.p))
            {
                HRESULT hr = (pIEvents->*func)(std::forward<Args>(args)...);
                OnFireResult(nConnectionIndex, hr);
            }
        }
    }

private:
    virtual void OnFireResult(int /*index*/, HRESULT /*hr*/) {}
};

} // details
} // com 
} // cpp
