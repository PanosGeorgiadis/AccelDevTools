// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <string>
#include <map>
#include <cpplib/link_cpplib.h>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/com_exception.h>

namespace cpp
{
namespace com
{

class CheckedHResult
{
public:
	CheckedHResult() = delete;
	virtual ~CheckedHResult() = default;
	CheckedHResult(const CheckedHResult&) = delete;
	CheckedHResult& operator=(const CheckedHResult&) = delete;
    //TICS -INT#001 Implicit conversion by design, ease of use 
	CheckedHResult(HRESULT retval);
    //TICS +INT#001
    CheckedHResult(HRESULT retval, const std::string& description);  
	void operator=(const HRESULT retval);
    HRESULT get() const;

private:
	std::string ToString(HRESULT hr) const;
	std::string GetErrorInfoDescription(IErrorInfo* pIErrorInfo) const;
	void Check(HRESULT retval);

    static std::map<HRESULT, std::string> m_errorcode_map;

    HRESULT m_hr;
};

} // com 
} // cpp