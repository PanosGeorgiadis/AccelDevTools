// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <functional>

namespace cpp
{
namespace win32
{

//-----------------------------------------------------------------------------------------------------------------

template <typename T, typename Deleter>
class ApiPtr
{
public:
    ApiPtr() : 
        m_ptr{nullptr} 
    {
    };

	ApiPtr(T* ptr) : 
        m_ptr{ptr}
    {
    };

	ApiPtr(const ApiPtr&) = delete;
	ApiPtr& operator=(const ApiPtr&) = delete;

	operator T*() const 
    { 
        return m_ptr; 
    }

	T** operator&() throw()
	{
		return &m_ptr;
	}

	T* get() const
	{
		return m_ptr;
	}

	~ApiPtr()
	{
		Deleter deleter;
		if (m_ptr != nullptr)
		{
			deleter(m_ptr);
		}
	}

private:
	T* m_ptr;
};

template <typename T>
class ApiPtrFn
{
public:
	ApiPtrFn() = delete;

	ApiPtrFn(std::function<void (T*)> deleter) :
        m_ptr{nullptr},  
        m_deleter{deleter}
	{
	}

	ApiPtrFn(const ApiPtrFn&) = delete;
	ApiPtrFn& operator=(const ApiPtrFn&) = delete;

	operator T*() const { return m_ptr; }
	T** operator&() throw()
	{
		return &m_ptr;
	}

	~ApiPtrFn()
	{
		if (m_ptr != nullptr)
		{
			m_deleter(m_ptr);
		}
	}

private:
	T* m_ptr;
	std::function<void(T*)> m_deleter;
};


} // win32
} // cpp