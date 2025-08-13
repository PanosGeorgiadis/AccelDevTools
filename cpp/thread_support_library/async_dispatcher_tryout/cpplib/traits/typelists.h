// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <type_traits>

namespace cpp
{
namespace traits
{

namespace debug
{
template<typename T> struct debug_t;
} // debug

struct InvalidType : std::false_type {};

template <typename... Ts>
struct TypeList
{
};

template<>
struct TypeList<>
{
};

template<typename... Ts>
constexpr bool IsEmpty() { return (sizeof...(Ts) == 0); }

template<typename... Ts>
constexpr bool HasTypes() { return (sizeof...(Ts) != 0); }


//---------------------------------------------------------------------------------------------------------------------

template <typename List, typename T>
struct Prepend;

template <typename ... ListItems, typename T>
struct Prepend<T, TypeList<ListItems...>>
{
	using T_t = typename T;
    using type = TypeList<T_t, ListItems...>;
};

template <typename List, typename T>
using Prepend_t = typename Prepend<T, List>::type;

//---------------------------------------------------------------------------------------------------------------------

template<typename... Ts>
struct Splitter
{
private:
	//static constexpr size_t Last = (sizeof...(Ts) - 2);

	template<size_t pos, typename... Tail>	struct recurse {};

	template<>
	struct recurse<0>
	{
		using head = TypeList<>;
		using tail = void;			// return
	};
	
	template<typename Head>
	struct recurse<1, Head>
	{
		using head = typename TypeList<>;
		using tail = typename Head;
	};

	template<size_t pos, typename Head, typename... Tail>
	struct recurse<pos, Head, Tail...>
	{
		using result = recurse<pos - 1, Tail...>;

		using partial_list = typename result::head;
		using head_t = typename Head;

		using head = Prepend_t<partial_list,head_t>;
		using tail = typename result::tail;
	};

public:
	using result = recurse<sizeof...(Ts), Ts...>;
	using head = typename result::head;
	using tail = typename result::tail;
};

template<typename TypeList>
struct SplitOfLastArg {};

template<template <typename...> typename TypeList, typename...Ts>
struct SplitOfLastArg<TypeList<Ts...>>
{
	using head = typename Splitter<Ts...>::head;
	using tail = typename Splitter<Ts...>::tail;
};

} // traits
} // cpp
