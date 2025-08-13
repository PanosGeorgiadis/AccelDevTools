// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
#pragma once

#include "active_proxy_tracker.h"
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/checked_hresult.h>
#include <cpplib/com/com_object.h>
#include <cpplib/types/unreferenced_variables.h>

namespace cpp
{
namespace com
{

template <class Base>
class ComTrackedProxyObject :
    public Base,
    public IExternalConnection,
    public TrackedProxyItf
{
public:
    template <typename... Arguments>
    ComTrackedProxyObject(ActiveProxyTracker& proxyTracker, Arguments&&... args) :
        Base{ std::forward<Arguments>(args)... },
        m_proxyTracker{ proxyTracker },
        m_externalConnectionCounter(0u)
    {
        m_proxyTracker.AddProxy(this, ExtractTypeName(typeid(Base).name()));
    }

    virtual ~ComTrackedProxyObject()
    {
        m_proxyTracker.RemoveProxy(this);
    }

    virtual void Disconnect() //override
    {
        ::CoDisconnectObject(GetUnknown(), 0u);
    }

    // workaround, normal chain macro won't compile somehow
#define COM_INTERFACE_ENTRY_CHAIN_FIX(classname)\
	{NULL,\
	(DWORD_PTR)&ATL::_CComChainData<classname, _ComMapClass>::data,\
	Base::_Chain},

    BEGIN_COM_MAP(ComTrackedProxyObject<Base>)
        COM_INTERFACE_ENTRY(IExternalConnection)
        COM_INTERFACE_ENTRY_CHAIN_FIX(Base)
    END_COM_MAP()

    void FinalRelease()
    {
        Disconnect();
        Base::FinalRelease();
    }

    // IExternalConnection implementation
    STDMETHODIMP_(DWORD) AddConnection(_In_ DWORD extconn, _In_ DWORD reserved)
    {
        unreferenced::parameter(reserved);

        DWORD result(0);

        if (0 != (extconn & EXTCONN_STRONG))
        {
            LockGuard lock(m_mutex);
            result = ++m_externalConnectionCounter;

            if (1 == result)
            {
                m_proxyTracker.OnExternalClientConnectsToProxy(this);
            }
        }

        return result;
    }

    STDMETHODIMP_(DWORD) ReleaseConnection(_In_ DWORD extconn, _In_ DWORD reserved, _In_ BOOL fLastReleaseCloses)
    {
        unreferenced::parameter(reserved);

        DWORD res(0);

        if (0 != (extconn & EXTCONN_STRONG))
        {
            LockGuard lock(m_mutex);

            res = --m_externalConnectionCounter;

            if ((0 == res) && (fLastReleaseCloses))
            {
                m_proxyTracker.OnLastExternalClientDisconnectsFromProxy(this);
            }
        }

        return res;
    }

private:

    static std::string ExtractTypeName(const std::string& typeName)
    {
        const char whitespace = ' ';
        std::size_t pos = typeName.find_first_of(whitespace);

        if (std::string::npos != pos)
        {
            pos = typeName.find_first_not_of(whitespace, pos);
        }

        return typeName.substr(pos);
    }

    ActiveProxyTracker& m_proxyTracker;
    unsigned long m_externalConnectionCounter;
    using Mutex = std::recursive_mutex;
    using LockGuard = std::lock_guard<Mutex>;
    mutable Mutex m_mutex;
};

//  A ref counted pointer to a ComTrackedProxyObject that can be initialised from a ComObject and copied:
template <typename T>
class ComTrackedProxyObjectPtr
{
public:
    ComTrackedProxyObjectPtr()
        : m_pObject()
    {
    }

    ComTrackedProxyObjectPtr(T* ptr)
        : m_pIUnkown(ptr->GetUnknown())
        , m_pObject(ptr)
    {
    }

    template <typename U>
    ComTrackedProxyObjectPtr(const ComTrackedProxyObject<U>& obj) :
        m_pIUnkown(obj.GetInterface<IUnknown>()),
        m_pObject(obj.get())
    {
    }

    template <typename U>
    ComTrackedProxyObjectPtr(const ComTrackedProxyObjectPtr<U>& obj) :
        m_pIUnkown(obj.GetInterface<IUnknown>()),
        m_pObject(obj.get())
    {
    }

    T* get() const
    {
        return m_pObject;
    }

    T* operator->() const
    {
        assert(m_pObject); // this ComTrackedProxyObjectPtr was not initialized with a ComTrackedProxyObject
        return m_pObject;
    }

    template <typename Itf>
    ATL::CComPtr<Itf> GetInterface() const
    {
        ATL::CComPtr<Itf> pItf;
        CheckedHResult chr = m_pIUnkown->QueryInterface(&pItf);
        return pItf;
    }

private:
    ATL::CComPtr<IUnknown> m_pIUnkown;
    T* m_pObject;
};

template <typename T, typename... Arguments>
ComTrackedProxyObjectPtr<T> CreateComTrackedProxyObject(ActiveProxyTracker& proxyTracker, Arguments&&... args)
{
    return ComTrackedProxyObjectPtr<T>(new CComObjectEx<ComTrackedProxyObject<T>>(proxyTracker, std::forward<Arguments>(args)...));
}

} // com
} // cpp 
