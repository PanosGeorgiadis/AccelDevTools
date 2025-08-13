// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
//

#pragma once

#include <type_traits>

namespace cpp
{
namespace com
{
namespace traits
{

template<typename T, typename = void>
struct is_iterator : std::false_type { };

template<typename T>
struct is_iterator<T*, void> : std::true_type {};

template<typename T>
struct is_iterator<T,
    typename std::enable_if<
    !std::is_same<typename T::value_type, void>::value &&
    !std::is_same<typename T::reference, void>::value &&
    !std::is_same<typename T::difference_type, void>::value &&
    !std::is_same<typename T::pointer, void>::value &&
    !std::is_same<typename T::iterator_category, void>::value
    >::type> : std::true_type {};

template<typename T, typename = void>
struct iterator_value_type { };

template<typename T>
struct iterator_value_type<T, typename std::enable_if<is_iterator<T>::value>::type>
{
    using type = typename std::iterator_traits<T>::value_type;
};

} // traits
} // com 
} // cpp