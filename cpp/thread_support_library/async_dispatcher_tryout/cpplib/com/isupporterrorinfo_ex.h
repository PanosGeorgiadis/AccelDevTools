// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/com/atlcom_api.h>
#include <cpplib/traits/typelists.h>
#include <tuple>

namespace cpp
{
namespace com
{

template<typename... Interfaces>
struct ATL_NO_VTABLE ISupportErrorInfoImplEx :
    public ISupportErrorInfo
{
    STDMETHODIMP InterfaceSupportsErrorInfo(REFIID riid)
    {
        using impl = supported_impl<sizeof...(Interfaces), Interfaces...>;
        return impl::supports(riid);
    }

private:
    template<size_t size_left, typename... Itfs> struct supported_impl;

    template<>
    struct supported_impl<0>
    {
        static inline HRESULT supports(REFIID /*riid*/)
        {
            return S_FALSE;
        }
    };

    template<typename Itf>
    struct supported_impl<1, Itf>
    {
        static inline HRESULT supports(REFIID riid)
        {
            return InlineIsEqualGUID(__uuidof(Itf), riid) ? S_OK : S_FALSE;
        }
    };

    template<size_t size_left, typename Itf, typename... Itfs>
    struct supported_impl<size_left, Itf, Itfs...>
    {
        static inline HRESULT supports(REFIID riid)
        {
            if (supported_impl<1, Itf>::supports(riid) == S_OK)
            {
                return S_OK;
            }

            return supported_impl<size_left - 1, Itfs...>::supports(riid);
        }
    };

};

} // com
} // cpp
