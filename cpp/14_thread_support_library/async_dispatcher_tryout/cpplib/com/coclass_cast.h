// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <cpplib/com/com_base.h>

// This cast allows you to cast from a COM Interface pointer to the implementing CoClass
// NOTE: this will only work at runtime when both the interface and coclass are in the same process and apartment (MTA)

namespace cpp
{
namespace com
{

interface __declspec(uuid("{6B8FF54A-BE2C-41A3-B9A1-5C443A1E14BB}")) ISupportCoClassCast : IUnknown
{
};

// This trick deserves some explanantion:
// Step 1: The ISupportCoClassCast interface is never registered and can not be proxied
// Step 2: A coclass that supports coclass_cast needs to implement ISupportCoClassCast
// Step 3: Use QueryInterface() to get a direct pointer to the ISupportCoClassCast base class or an E_NOINTERFACE error.
// Step 3a: In case of _ATL_DEBUG_INTERFACES, the QI thunk's m_pUnk points to the actual ISupportCoClassCast base class.
// Step 4: Once we have an ISupportCoClassCast*, dynamic_cast() will get us to any legal derived class, including the coclass implementation

template <typename CoClassPtr>
CoClassPtr coclass_cast(IUnknown* pIUnk)
{
    if (pIUnk == nullptr)
    {
        return nullptr;
    }

    auto pILocal = com_cast<ISupportCoClassCast>(pIUnk);
    if (pILocal == nullptr)
    {
        throw std::invalid_argument("not a local interface");
    }

    if (auto p = dynamic_cast<CoClassPtr>(pIUnk))
    {
        return p;
    }

    throw std::bad_cast::__construct_from_string_literal("bad coclass_cast");
}

} // com
} // cpp