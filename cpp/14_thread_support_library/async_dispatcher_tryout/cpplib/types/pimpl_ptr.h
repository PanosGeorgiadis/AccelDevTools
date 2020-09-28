// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <memory>

namespace cpp
{
namespace pimpl
{
namespace details
{

template<typename T>
void impl_deleter(T *p) noexcept
{
	static_assert(sizeof(T) > 0, "cannot delete incomplete type");
	static_assert(!std::is_void<T>::value, "cannot delete incomplete type");
	delete p;
}

} // details

// Pointer to unique implementation
template<typename T, typename Deleter = void(*)(T*)> 
using ptr = std::unique_ptr<T, Deleter>;

// Constructs an object of type T and wraps it and related default deleter in `unique_impl_ptr`
template<typename T, typename... Args>
inline ptr<T> make_ptr(Args&&... args)
{
	static_assert(!std::is_array<T>::value, "arrays are not supported");
	return ptr<T>(new T(std::forward<Args>(args)...), &details::impl_deleter<T>);
}

} // pimpl
} // cpp