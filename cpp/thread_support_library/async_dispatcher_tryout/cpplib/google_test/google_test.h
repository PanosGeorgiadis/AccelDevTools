// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <stdexcept>
#include <codeanalysis/warnings.h>

//TICS -POR#021 explicitly disable warnings 
#pragma comment(lib,"gtest.lib")
#pragma warning(push)
#pragma warning(disable:4996)
#pragma warning(disable:6326)
#pragma warning(disable:ALL_CODE_ANALYSIS_WARNINGS)
#define GTEST_LANG_CXX11 1
#include <gtest/gtest.h>
#pragma warning(pop)