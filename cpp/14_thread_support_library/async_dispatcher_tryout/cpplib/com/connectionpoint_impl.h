// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <type_traits>
#include <cpplib/com/traits.h>
#include <cpplib/com/details/disp_connectionpoint_impl.h>
#include <cpplib/com/details/custom_connectionpoint_impl.h>

namespace cpp
{
namespace com
{

template <typename T, typename IEvents, typename CDV = ATL::CComDynamicUnkArray>
class IConnectionPointImpl :
    public std::conditional<
        is_dispatch_interface<IEvents>::value,
        details::IDispConnectionPointImpl<T, IEvents, CDV>,
        details::IUnkConnectionPointImpl<T, IEvents, CDV>
    >::type
{
protected:
    IConnectionPointImpl() = default;
    virtual ~IConnectionPointImpl() = default;
    IConnectionPointImpl(const IConnectionPointImpl&) = delete;
    IConnectionPointImpl& operator=(const IConnectionPointImpl&) = delete;
};

} // com 
} // cpp
