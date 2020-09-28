// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

struct IUnknown;
#include <rpc.h>
#include <cpplib/link_cpplib.h>

namespace cpp
{
namespace win32
{
namespace details
{

class checked_rpc_status
{
public:
	checked_rpc_status() = delete;
	virtual ~checked_rpc_status() = default;
	checked_rpc_status(const checked_rpc_status&) = delete;
	checked_rpc_status& operator=(const checked_rpc_status&) = delete;
	checked_rpc_status(RPC_STATUS retval);  //TICS !INT#001 implicit by design to be able to write one-liner checks!
	void operator=(const RPC_STATUS retval);
private:
	void check(RPC_STATUS retval);
};

} // details
} // win32
} // cpp
