// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <memory>
#include <string>
#include <cpplib/types/interface.h>
#include <cpplib/types/null_object.h>
#include <cpplib/types/pimpl_ptr.h>

namespace cpp
{
namespace performance
{

#ifdef COMPILING_CPPLIB_PERF_COUNTERS
#define CPPLIB_PERF_COUNTERS_EXPORT __declspec(dllexport)
#else
#define CPPLIB_PERF_COUNTERS_EXPORT __declspec(dllimport)
#endif

//-----------------------------------------------------------------------------------------------------------------------------------------

namespace impl
{
class CpplibPerformanceImpl;
}

//-----------------------------------------------------------------------------------------------------------------------------------------

class DispatcherPerformanceItf :
    public Interface
{
public:
    virtual void increment_number_of_calls_queued() = 0;
    virtual void report_queue_size(std::uint32_t) = 0;
    virtual void report_latency_in_milliseconds(std::uint32_t) = 0;
};

class NullDispatcherPerformance :
    public NullObject<DispatcherPerformanceItf, NullDispatcherPerformance>
{
public:
    virtual void increment_number_of_calls_queued() override {}
    virtual void report_queue_size(std::uint32_t) override {}
    virtual void report_latency_in_milliseconds(std::uint32_t) override {}
};

//-----------------------------------------------------------------------------------------------------------------------------------------

class CpplibPerformanceItf :
    public Interface
{
public:
    virtual CPPLIB_PERF_COUNTERS_EXPORT  std::shared_ptr<DispatcherPerformanceItf> GetDispatcherPerformanceCounters(const std::wstring& instance_name) = 0;
};

//-----------------------------------------------------------------------------------------------------------------------------------------

// CPPLIB_PERF_COUNTERS_EXPORT only export methods useable by clients!
// no need to export the whole class (avoids linking problems with pimpl::ptr to boot
class CpplibPerformance final :
    public CpplibPerformanceItf
{
public:
    CPPLIB_PERF_COUNTERS_EXPORT CpplibPerformance();
    CPPLIB_PERF_COUNTERS_EXPORT ~CpplibPerformance();
    CpplibPerformance(const CpplibPerformance&) = delete;
    CpplibPerformance& operator=(const CpplibPerformance&) = delete;

    CPPLIB_PERF_COUNTERS_EXPORT virtual std::shared_ptr<DispatcherPerformanceItf> GetDispatcherPerformanceCounters(const std::wstring& instance_name) override;

private:
    pimpl::ptr<impl::CpplibPerformanceImpl> m_pimpl;
};

}
}