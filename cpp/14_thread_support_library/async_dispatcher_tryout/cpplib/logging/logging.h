// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
//
// Abstract baseclass for connecting to INFRA logging
// use it if you need a lot of different levels/categories for logging. Otherwise define your own "smaller" interface to logging from your library
//

#pragma once

#include <string>
#include <cstdint>
#include <cpplib/types/bitfield_enum.h>
#include <cpplib/types/interface.h>
#include <cpplib/types/null_object.h>

namespace cpp
{

enum class Severity
{
    Informational,
    Warning,
    Error,
    Fatal
};

enum class Category : std::uint32_t 
{
    StateChange = 0x00000001,
    Parameter = 0x00000002,
    MethodInvocation = 0x00000004,
    MethodParameters = 0x00000008,
    Synchronization = 0x00000010,
    Notification = 0x00000020,
    InternalParameter = 0x00000040,
    Initialization = 0x00000080,
    Configuration = 0x00000100,
    Communication = 0x00000200,
    Hardware = 0x00000400,
    InternalError = 0x00001000,
    DecisionPoint = 0x00002000,
    ProcessFlow = 0x00000800,
    EndUser = 0x00100000,
    Service = 0x00200000,
    Manufacturing = 0x00400000,
    ForceEventLog = 0x00800000,
    Developer1 = 0x01000000,
    Developer2 = 0x02000000,
    Developer3 = 0x04000000,
    Developer4 = 0x08000000,
    Developer5 = 0x10000000,
    Developer6 = 0x20000000,
    Developer7 = 0x40000000,
    Developer8 = 0x80000000
};

class LoggingItf :
    public Interface
{
public:
    virtual void log(Severity sev, Category cat, const std::wstring& message) const noexcept = 0;
};

class NullLogger :
    public NullObject<LoggingItf, NullLogger>
{
    virtual void log(Severity, Category, const std::wstring&) const noexcept override {}
};

namespace details
{
// enable bit field operators on LogCategory enum
template<>
struct is_bitfield_enum<Category> : public std::true_type {};
}

} // cpp
