// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <functional>

namespace cpp
{

template<typename T>
struct memfun_type
{
	using type = void;
};

template<typename Ret, typename Class, typename... Args>
struct memfun_type<Ret(Class::*)(Args...) const>
{
	using type = std::function<Ret(Args...)>;
};

template<typename L>
typename memfun_type<decltype(&L::operator())>::type to_function(L const &func)
{
	return func;
}

} // cpp