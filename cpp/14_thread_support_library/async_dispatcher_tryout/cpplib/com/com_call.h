// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/com/atlcom_api.h>
#include <cpplib/traits/typelists.h>
#include <cpplib/com/checked_hresult.h>
#include <cpplib/com/details/wrapper.h>
#include <cpplib/preprocessor/stringify.h>

namespace cpp
{
namespace com
{
namespace details
{

template <typename Fn>
struct ComCallArgTypes
{
    using retval_type = typename traits::InvalidType;
    using args_typelist = typename traits::TypeList<>;
};

template <typename RetvalType, typename Itf, typename... Args>
struct ComCallArgTypes <RetvalType(STDMETHODCALLTYPE Itf::*)(Args...) >
{
    static constexpr auto Size = sizeof...(Args);
    using result = traits::SplitOfLastArg<traits::TypeList<Args...>>;
    using retval_type = std::remove_pointer_t<typename result::tail>;
};

template <typename RetvalType, typename Itf, typename... Args>
struct ComCallArgTypes <COM_DECLSPEC_NOTHROW RetvalType(STDMETHODCALLTYPE Itf::*)(Args...) >
{
    static constexpr auto Size = sizeof...(Args);
    using result = traits::SplitOfLastArg<traits::TypeList<Args...>>;
    using retval_type = std::remove_pointer_t<typename result::tail>;
};

template<typename Itf, typename Fn>
struct ComCallWrapper
{
    using ArgTypes = typename ComCallArgTypes<Fn>;
    static constexpr auto Size = ArgTypes::Size;
    using RetvalType = typename ArgTypes::retval_type;
    using ComRetvalType = typename ComWrapper<typename ArgTypes::retval_type>::type;

    ComCallWrapper(Itf* itf, Fn fn) :
        m_itf{ itf },
        m_fn{ fn }
    {
    }

    ComCallWrapper(const ATL::CComPtr<Itf>& itf, Fn fn) :
        m_itf{ itf },
        m_fn{ fn }
    {
    }

    ComCallWrapper(const ATL::CComQIPtr<Itf>& itf, Fn fn) :
        m_itf{ itf },
        m_fn{ fn }
    {
    }

    template<typename... Args>
    typename std::enable_if_t<sizeof...(Args) == ArgTypes::Size, void> operator()(Args&&... args) const
    {
        CheckedHResult chr = (m_itf->*m_fn)(std::forward<Args>(args)...);
    }

    template<typename... Args>
    typename std::enable_if_t<sizeof...(Args) == (ArgTypes::Size - 1), ComRetvalType> operator()(Args&&... args) const
    {
        ComRetvalType retval{};
        CheckedHResult chr = (m_itf->*m_fn)(std::forward<Args>(args)..., &retval);
        return retval;
    }

    Itf* m_itf;
    Fn m_fn;
};

} // details

// CPP_COM_CALL will only work with CComPtr/CComQIPtr. 
#define CPP_COM_CALL(ptr,fun) details::ComCallWrapper((ptr),&decltype(ptr)::_PtrClass::fun)

} // com 
} // cpp

