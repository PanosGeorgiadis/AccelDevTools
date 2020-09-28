// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/logging/logging.h>

namespace cpp
{
namespace test
{

class LogginMock :
    public LoggingItf
{
    virtual void log(Severity sev, Category cat, const std::wstring& message) const noexcept = 0;
    //MOCK_METHOD3(log, void(Severity, Category, const std::wstring&);
};


}
}
