// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <cpplib/com/atlcom_api.h>

namespace cpp
{
namespace com
{

struct MultithreadedApartment
{
    MultithreadedApartment()
    {
        HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (FAILED(hr)) throw std::runtime_error("com failed to initialize");
    }

    ~MultithreadedApartment()
    {
        ::CoUninitialize();
    }
};

struct MultithreadedApartmentNoDDE
{
    MultithreadedApartmentNoDDE()
    {
        HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) throw std::runtime_error("com failed to initialize");
    }

    ~MultithreadedApartmentNoDDE()
    {
        ::CoUninitialize();
    }
};

struct SingleThreadedApartment
{
    SingleThreadedApartment()
    {
        HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) throw std::runtime_error("com failed to initialize");
    }

    ~SingleThreadedApartment()
    {
        ::CoUninitialize();
    }
};

struct SingleThreadedApartmentNoDDE
{
    SingleThreadedApartmentNoDDE()
    {
        HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) throw std::runtime_error("com failed to initialize");
    }

    ~SingleThreadedApartmentNoDDE()
    {
        ::CoUninitialize();
    }
};

} // com 
} // cpp

