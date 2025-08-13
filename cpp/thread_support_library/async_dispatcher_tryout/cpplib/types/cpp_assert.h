// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cassert>
#include <cpplib/types/unreferenced_variables.h>

#ifdef _DEBUG
#define cpp_assert(x) assert(x)
#else
#define cpp_assert(x) unreferenced::parameter(x)
#endif


