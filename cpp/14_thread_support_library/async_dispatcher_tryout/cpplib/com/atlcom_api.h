// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
//
// Include this file if you want to use ATL
//

#pragma once

struct IUnknown;		// workaround for undefined IUnknown in some vs2017 versions

#define _ATL_NO_AUTOMATIC_NAMESPACE

#include <cpplib/win32/win_api.h>
#include <winerror.h>
#include <OAIdl.h>
#include <OleAuto.h>
#include <OCIdl.h>
#include <combaseapi.h>
#include <comdef.h>
#include <comutil.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlcomcli.h>
#include <atlsafe.h>

