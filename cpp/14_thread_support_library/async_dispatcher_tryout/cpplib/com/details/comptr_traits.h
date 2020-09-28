// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

// Extract COM interface type from an interface raw or smart pointer:
//
// com_ptr_traits<PointerType>::type

#pragma once
#include <type_traits>

// Specializations for a raw pointer and a smart pointer.
// The interface type of the T* specialization is just T
// The interface type of the smart pointer specialization is the return type of its operator->() (which makes it a smart pointer)

namespace cpp
{
namespace com
{
namespace details
{

template <typename T, typename Enable = void> struct com_ptr_traits;

template <typename T>
struct com_ptr_traits<T*>
{
    using interface_type = T;

    static IID iid()
    {
        return __uuidof(interface_type);
    }
};

template <typename T>
struct RemoveNoAddRefReleaseOnCComPtr
{
    using type = T;
};

template <typename T>
struct RemoveNoAddRefReleaseOnCComPtr<T&> : RemoveNoAddRefReleaseOnCComPtr<T>
{
};

template <typename T>
struct RemoveNoAddRefReleaseOnCComPtr<ATL::_NoAddRefReleaseOnCComPtr<T>> : RemoveNoAddRefReleaseOnCComPtr<T>
{
};

template <typename T>
struct com_ptr_traits<T, typename std::enable_if<std::is_class<T>::value>::type>
{
    using interface_type = typename RemoveNoAddRefReleaseOnCComPtr<decltype(*static_cast<T*>(0)->operator->())>::type;

    static IID iid()
    {
        return __uuidof(interface_type);
    }
};

template <typename T>
struct com_ptr_traits<T&> : com_ptr_traits<T>
{
};

} // details
} // com 
} // cpp

