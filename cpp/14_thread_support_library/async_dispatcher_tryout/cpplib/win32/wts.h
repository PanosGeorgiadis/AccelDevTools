// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <cpplib/win32/win_api.h>
#include <wtsapi32.h>
#pragma comment(lib,"Wtsapi32.lib")

#include "api_ptr.h"
#include "api_handle.h"

namespace cpp
{
namespace win32
{

namespace details
{
struct session_info_deleter
{
	void operator()(WTS_SESSION_INFO* ptr) noexcept
	{
		::WTSFreeMemory(ptr);
	}
};

struct wts_free_memory
{
	void operator()(void* ptr) noexcept
	{
		::WTSFreeMemory(ptr);
	}
};

} // namespace details

using wts_info_ptr = ApiPtr<WTS_SESSION_INFO, details::wts_free_memory>;
using wts_connectstate_ptr = ApiPtr<WTS_CONNECTSTATE_CLASS, details::wts_free_memory>;
using wts_wstring_ptr = ApiPtr<WCHAR, details::wts_free_memory>;

} // win32
} // cpp