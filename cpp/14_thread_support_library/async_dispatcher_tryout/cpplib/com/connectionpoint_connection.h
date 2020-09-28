// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

// Maintain a ConnectionPoint connection between source and sink objects.
// ConnectionPointConnection ctor does Advise(), throws HResultException for COM errors
// ConnectionPointConnection dtor does Unadvise()

#pragma once

#include <cpplib/link_cpplib.h>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/com_base.h>
#include <cpplib/com/com_call.h>
#include <cpplib/types/non_copyable.h>

namespace cpp
{
namespace com
{

class ConnectionPointConnection : NonCopyable
{
public:
    template <typename SourceItf, typename EventItf>
    ConnectionPointConnection(const ATL::CComPtr<SourceItf>& source, const ATL::CComPtr<EventItf>& sink) 
    {
        ATL::CComPtr<IUnknown> sink_unknown = GetInterface<IUnknown>(sink);
        ATL::CComPtr<IConnectionPointContainer> connectionpoint_container = GetInterface<IConnectionPointContainer>(source);
        m_pICp = CPP_COM_CALL(connectionpoint_container,FindConnectionPoint)(__uuidof(EventItf));
        m_cookie = CPP_COM_CALL(m_pICp,Advise)(sink_unknown);
    }

    virtual ~ConnectionPointConnection()
    {
        try
        {
            CPP_COM_CALL(m_pICp,Unadvise)(m_cookie);
        }
        catch (const com_exception&)
        {
            assert(false);
        }
    }

private:
    ATL::CComPtr<IConnectionPoint> m_pICp;
    DWORD m_cookie;
};

} // com
} // cpp
