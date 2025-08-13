// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once
#include <memory>

namespace cpp
{

template<typename Itf, typename T>
class NullObject :
    public Itf
{
public:
    NullObject() = default;
    virtual ~NullObject() = default;
    NullObject(const NullObject&) = delete;
    NullObject& operator=(const NullObject&) = delete;
    NullObject(NullObject&&) = delete;

    static Itf& instance()
    {
        static T obj;
        return obj;
    }

    static std::unique_ptr<Itf> unique()
    {
        return std::make_unique<T>();
    }

    static std::shared_ptr<Itf> shared()
    {
        return std::make_shared<T>();
    }
};

} // cpp
