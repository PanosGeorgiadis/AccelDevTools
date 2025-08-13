// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <iterator>
#include <type_traits>

namespace cpp
{

#include <type_traits>
template <typename enum_t, enum_t first, enum_t last>
class enum_iterator
{
	typedef typename std::underlying_type<C>::type val_t;

public:
	Iterator(const enum_t &f) : 
		val(static_cast<val_t>(f)) {}
	
	Iterator() : val_t(first) 
	{
	}
	Iterator operator++() {
		++val;
		return *this;
	}
	C operator*() { return static_cast<C>(val); }
	Iterator begin() { return *this; } //default ctor is good
	Iterator end() {
		static const Iterator endIter = ++Iterator(endVal); // cache it
		return endIter;
	}
	bool operator!=(const Iterator& i) { return val != i.val; }

private:
	m_val_t val;
};


};
