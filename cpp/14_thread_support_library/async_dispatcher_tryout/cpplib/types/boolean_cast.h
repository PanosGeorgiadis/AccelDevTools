// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <type_traits>

#ifndef __wtypes_h__
struct IUnknown;
#include <wtypes.h>
#endif

namespace cpp
{
namespace details
{

using variant_bool = short;

template <typename T>
struct boolean_traits
{
	static const T false_value = false;
	static const T true_value = true;

	static bool to_bool(T v)
	{
		return v ? true: false;
	}
};

template <typename T>
struct boolean_traits<T*>
{
	static bool to_bool(T* v)
	{
		return v != nullptr;
	}
};

template <>
struct boolean_traits<variant_bool>
{
	static const variant_bool false_value = 0;
	static const variant_bool true_value = static_cast<variant_bool>(-1);

	static bool to_bool(variant_bool v)
	{
		return v != false_value;
	}
};

#ifdef WINAPI
static_assert(
	std::is_same<variant_bool, VARIANT_BOOL>::value &&
	boolean_traits<variant_bool>::false_value == VARIANT_FALSE &&
	boolean_traits<variant_bool>::true_value == VARIANT_TRUE, "Unexpected VARIANT_BOOL type");
#endif

} // details

template <typename TargetType, typename SourceType>
TargetType boolean_cast(SourceType s)
{
	return details::boolean_traits<SourceType>::to_bool(s) ? details::boolean_traits<TargetType>::true_value : details::boolean_traits<TargetType>::false_value;
}

} // cpp


