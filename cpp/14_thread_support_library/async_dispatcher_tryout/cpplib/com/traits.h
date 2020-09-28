// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <type_traits>
#include <cpplib/com/atlcom_api.h>

namespace cpp
{
namespace com
{
template <typename T> struct is_com_interface : std::is_base_of<IUnknown, T> { };
template <typename T> struct is_com_interface_pointer : std::false_type { };
template <typename T> struct is_com_interface_pointer<T*> : is_com_interface<T> { };
template <typename T> struct is_dispatch_interface : std::is_base_of<IDispatch, T> { };
template <typename T> struct is_dispatch_interface_pointer : std::false_type { };
template <typename T> struct is_dispatch_interface_pointer<T*> : is_dispatch_interface<T> { };

template <typename T>
struct is_automation_type :
    std::integral_constant<bool,
    std::is_arithmetic<T>::value ||
    std::is_enum<T>::value ||
    is_com_interface_pointer<T>::value ||
    std::is_same<T, BSTR>::value ||
    std::is_same<T, VARIANT>::value ||
    std::is_same<T, SAFEARRAY*>::value ||
    std::is_same<T, CURRENCY>::value ||
    std::is_same<T, DATE>::value ||
    std::is_same<T, DECIMAL>::value ||
    std::is_same<T, GUID>::value>
{
};

}
}


