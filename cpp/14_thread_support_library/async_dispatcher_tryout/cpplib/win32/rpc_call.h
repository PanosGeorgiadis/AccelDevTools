// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include "api_error.h"
#include <cpplib/win32/details/checked_rpc_status.h>

namespace cpp
{
namespace win32
{
	template<typename Fn, typename... Args>
	void rpc_call(Fn fn, Args&&... args)
	{
		static_assert(std::is_same_v<decltype(fn(std::forward<Args>(args)...)), RPC_STATUS>, "Not an RPC call");

		auto lambda = [&fn, &args...] 
		{ 
			return fn(std::forward<Args>(args)...);
		};

		details::checked_rpc_status err = lambda();
	}

} // win32
} // cpp