// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <memory>
#include <functional>
#include <cpplib/com/checked_hresult.h>

namespace cpp
{
namespace com
{

class ExceptionTranslatorBase
{
public:
    virtual ExceptionTranslatorBase* Append(std::unique_ptr<ExceptionTranslatorBase>&& pFilter) = 0;
    virtual ExceptionTranslatorBase* Insert(std::unique_ptr<ExceptionTranslatorBase>&& pFilter) = 0;

    virtual com_exception GetExceptionInfo() const = 0;

    virtual ~ExceptionTranslatorBase()
    {
    }
};

template <typename Exception, typename Filter>
class ExceptionTranslator : public ExceptionTranslatorBase
{
public:
    explicit ExceptionTranslator(Filter f) :
        m_filter{ f }
    {
    }

    virtual ExceptionTranslatorBase* Append(std::unique_ptr<ExceptionTranslatorBase>&& pFilter)
    {
        m_pNext = std::move(pFilter);
        return m_pNext.get();
    }

    virtual ExceptionTranslatorBase* Insert(std::unique_ptr<ExceptionTranslatorBase>&& pFilter)
    {
        pFilter->Append(std::move(m_pNext));
        return Append(std::move(pFilter));
    }

    virtual com_exception GetExceptionInfo() const
    {
        try
        {
            return m_pNext->GetExceptionInfo();
        }
        catch (Exception& e)
        {
            return m_filter(e);
        }
    }

private:
    Filter m_filter;
    std::unique_ptr<ExceptionTranslatorBase> m_pNext;
};

void SetExceptionTranslator(std::unique_ptr<ExceptionTranslatorBase>&& pFilter);

template <typename Exception, typename Filter>
void SetExceptionTranslator(Filter f)
{
    std::unique_ptr<ExceptionTranslatorBase> pFilter(new ExceptionTranslator<Exception, Filter>(f));
    SetExceptionTranslator(std::move(pFilter));
}

com_exception GetExceptionInfo();
HRESULT HResultFromException();
HRESULT HResultFromException(const IID& iid);

} // com 
} // cpp

