// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/com/atlcom_api.h>

namespace cpp
{
namespace com
{
namespace details
{

template <typename T, typename IEvents, typename CDV = ATL::CComDynamicUnkArray>
class IDispConnectionPointImpl : public ATL::IConnectionPointImpl<T, &__uuidof(IEvents), CDV>
{
protected:
    IDispConnectionPointImpl() 
    {
        this->m_vec.clear();
    }

    virtual ~IDispConnectionPointImpl() = default;
    IDispConnectionPointImpl(const IDispConnectionPointImpl&) = delete;
    IDispConnectionPointImpl& operator=(IDispConnectionPointImpl&) = delete;
    //IDispConnectionPointImpl(IDispConnectionPointImpl&&) = delete;

    template<typename... Args>
    void Fire(DISPID id, Args&&... args)
    {
        ATL::CComVariant variant_args[] = { args... };
        FireImpl(id, variant_args, sizeof...(Args));
    }

    template<>
    void Fire(DISPID id)
    {
        FireImpl(id, nullptr, 0u);
    }

private:
    void FireImpl(DISPID dispIdMember, ATL::CComVariant args[], size_t cargs)
    {
        T* pT = static_cast<T*>(this);
        int nConnections = this->m_vec.GetSize();
        for (int nConnectionIndex = 0; nConnectionIndex < nConnections; ++nConnectionIndex)
        {
            pT->Lock();
            ATL::CComQIPtr<IDispatch> client(this->m_vec.GetAt(nConnectionIndex));
            pT->Unlock();

            if (client != nullptr)
            {
                DISPPARAMS disp = { args, nullptr, static_cast<unsigned>(cargs), 0 };
                ATL::CComVariant varResult;
                HRESULT hr = client->Invoke(dispIdMember, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, nullptr, nullptr);
                if (FAILED(hr) && hr != DISP_E_MEMBERNOTFOUND)
                {
                    this->Unadvise(nConnectionIndex + 1); // "cookie minus one is index into array" -- atlcom.h
#ifdef CPPLIB_CONNECTIONPOINT_ASSERT
                    assert(!"Unexpected exception from Connection Point client, removed");
#endif
                }
            }
        }

    };

};

} // details
} // com 
} // cpp