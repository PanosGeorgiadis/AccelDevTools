// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
#pragma once

#include <cpplib/google_test/google_test.h>
#include <cpplib/win32/win_api.h>
#include <cpplib/com/com_exception.h>

namespace testing
{

template <typename Fn>
void ExpectComException(HRESULT hr, Fn&& fn)
try
{
    fn();
}
catch (const cpp::com::com_exception& e)
{
    ASSERT_EQ(e.GetError(), hr);
}

} // testing