// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{
namespace unreferenced
{

#define UNREFERENED_FUNCTION static __forceinline 
#define UNREFERENCED_CALLTYPE __fastcall

template<typename Arg>
UNREFERENED_FUNCTION void UNREFERENCED_CALLTYPE parameter(Arg&&) noexcept {};

template<typename... Args>
UNREFERENED_FUNCTION void  UNREFERENCED_CALLTYPE parameters(Args&&...) noexcept {};

template<typename Arg>
UNREFERENED_FUNCTION void UNREFERENCED_CALLTYPE variable(Arg&&) noexcept {};

template<typename... Args>
UNREFERENED_FUNCTION void UNREFERENCED_CALLTYPE variables(Args&&...) noexcept {};

template<typename Arg>
UNREFERENED_FUNCTION void UNREFERENCED_CALLTYPE retval(Arg&&) noexcept {};
} // unreferenced

namespace unused
{
template<typename T>
UNREFERENED_FUNCTION void UNREFERENCED_CALLTYPE retval(const T& value) noexcept
{
    (void)value;
}
} // unused

} // cpp