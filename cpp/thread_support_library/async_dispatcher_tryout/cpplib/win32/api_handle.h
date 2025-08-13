// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <functional>

namespace cpp
{
namespace win32
{

//-----------------------------------------------------------------------------------------------------------------

class ApiHandle
{

public:
    ApiHandle() :
        m_handle{ INVALID_HANDLE_VALUE }
    {
    }

    ApiHandle(const ApiHandle&) = delete;
    ApiHandle& operator=(const ApiHandle&) = delete;

    operator const HANDLE*() const
    {
        return &m_handle;
    }

    HANDLE* operator&() throw()
    {
        return &m_handle;
    }

    ~ApiHandle()
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(m_handle);
        }
    }

private:
    HANDLE m_handle;
};

} // win32
} // cpp