// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <type_traits>
#include <cassert>
#include <cpplib/com/traits.h>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/types/non_copyable.h>
#include <cpplib/com/checked_hresult.h>
#include <cpplib/types/scope_guard.h>

namespace cpp
{
namespace com
{

template <typename Base>
class CComObjectEx
    : public Base
{
public:
    template<typename... Args>
    CComObjectEx(Args&&... args) :
        Base(std::forward<Args>(args)...)
    {
        static_assert(std::is_base_of_v<ATL::CComObjectRootBase, Base>, "Not derived from CComObjectRootBase");
        InitializeInstance();
    }

    // Set refcount to -(LONG_MAX/2) to protect destruction and 
    // also catch mismatched Release in debug builds
    virtual ~CComObjectEx() noexcept
    {
        this->m_dwRef = -(LONG_MAX / 2);
        this->FinalRelease();

#ifdef _ATL_DEBUG_INTERFACES
        _AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
#endif
        ATL::_pAtlModule->Unlock();
    }

    STDMETHOD_(ULONG, AddRef)()
    {
        return this->InternalAddRef();
    }

    STDMETHOD_(ULONG, Release)()
    {
        ULONG l = this->InternalRelease();
        if (l == 0)
        {
            delete this;
        }
        return l;
    }

    //if _InternalQueryInterface is undefined then you forgot BEGIN_COM_MAP
    STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject)
    {
        return this->_InternalQueryInterface(iid, ppvObject);
    }

    template <class Q>
    HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp)
    {
        return QueryInterface(__uuidof(Q), reinterpret_cast<void**>(pp));
    }

private:
    void InitializeInstance()
    {
        ATL::_pAtlModule->Lock();

        this->SetVoid(nullptr);
        this->InternalFinalConstructAddRef();

        SCOPE_GUARD([this]()
        {
            this->InternalFinalConstructRelease();
        });

        CheckedHResult chr = this->_AtlInitialConstruct();
        chr = this->FinalConstruct();
        chr = this->_AtlFinalConstruct();
    }
};

// Wrapper class for CComObjectEx<T> that handles instance creation and correct ref-counting
template <typename T>
class ComObject : 
    public NonCopyable
{
public:
    ComObject() :
        m_pObject(new CComObjectEx<T>())
    {
        m_pObject->AddRef();
    }

    explicit ComObject(CComObjectEx<T>* pObject) :
        m_pObject{ pObject }
    {
        m_pObject->AddRef();
    }

    ~ComObject()
    {
        m_pObject->Release();
    }

    template <typename Itf>
    ATL::CComPtr<Itf> GetInterface() const
    {
        ATL::CComPtr<Itf> pItf;
        CheckedHResult chr = m_pObject->QueryInterface(&pItf);
        return pItf;
    }

    CComObjectEx<T>* get() const
    {
        return m_pObject;
    }

    CComObjectEx<T>* operator->() const
    {
        return m_pObject;
    }

private:
    CComObjectEx<T>* m_pObject;
};

//  A ref counted pointer to a ComObject that can be initialised from a ComObject and copied:
template <typename T>
class ComObjectPtr
{
public:
    ComObjectPtr() : m_pObject{}
    {
    }

    explicit ComObjectPtr(T* ptr) :
        m_pIUnkown{ static_cast<IUnknown*>(ptr) },
        m_pObject{ ptr }
    {
    }

    template <typename U>
    ComObjectPtr(const ComObject<U>& obj) :
        m_pIUnkown{ obj.GetInterface<IUnknown>() },
        m_pObject{ obj.get() }
    {
    }

    template <typename U>
    ComObjectPtr(const ComObjectPtr<U>& obj) :
        m_pIUnkown{ obj.GetInterface<IUnknown>() },
        m_pObject{ obj.get() }
    {
    }

    T* get() const
    {
        return m_pObject;
    }

    T* operator->() const
    {
        assert(m_pObject != nullptr); // this ObjectPtr was not initialised with a ComObject
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

template <typename T1, typename T2>
bool operator==(const ComObjectPtr<T1>& p1, const ComObjectPtr<T2>& p2)
{
    return p1.get() == p2.get();
}

template <typename T1, typename T2>
bool operator!=(const ComObjectPtr<T1>& p1, const ComObjectPtr<T2>& p2)
{
    return !(p1 == p2);
}

template<typename T, typename... Args>
ComObjectPtr<T> CreateComObject(Args&&... args)
{
    static_assert(std::is_base_of_v<ATL::CComObjectRootBase, T>, "Object doesn't inherit from CComObjectRootEx");
    auto p = new CComObjectEx<T>(std::forward<Args>(args)...);
    return ComObject<T>(p);
}

} // com
} // cpp
