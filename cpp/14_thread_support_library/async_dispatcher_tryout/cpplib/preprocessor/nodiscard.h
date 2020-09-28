// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#if __cplusplus < 201500
#define NO_DISCARD 
#else
#define NO_DISCARD [[nodiscard]]
#endif