// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

namespace cpp
{

template<typename EnumT, typename BaseEnumT>
class ExtensibleEnum
{
public:
    ExtensibleEnum() = default;
    ~ExtensibleEnum() = default;

    ExtensibleEnum(EnumT value) :
        m_value{ value }
    {
    };

    ExtensibleEnum(BaseEnum value) :
        m_base_value{ value }
    {
    }

    operator const EnumT() const
    {
        return m_value;
    }

private:
    union
    {
        BaseEnum m_base_value;
        Enum m_value;
    }
};

} // cpp