// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
// 
// ComCollection.h: STL compatible wrapper for COM IEnumOnXxx collections.

// Example1:
//
// ComCollection<IMyCollection, IEnumVARIANT, CComBSTR> coll(pIColl);
// for (auto it = coll.begin(); it != coll.end(); ++it)
// {
//     std::wcout << *it << std::endl;
// }
//
//
// Example2:
//
// ComEnumIterator<IEnumVARIANT, CComBSTR> begin(pIEnum), end;
// while (begin != end)
// {
//     std::wcout << *begin << std::endl;
//     ++begin;
// }

#pragma once

#include <cassert>
#include <functional>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/com_call.h>
#include <cpplib/traits/identity.h>

namespace cpp
{
namespace com
{
namespace details
{

template <typename IEnum> struct ComEnumIteratorTraits;
template <typename Value> struct ComEnumIteratorTraitsImpl { using value_type = Value; };

template <> struct ComEnumIteratorTraits<IEnumVARIANT> : ComEnumIteratorTraitsImpl<ATL::CComVariant> {};
template <> struct ComEnumIteratorTraits<IEnumString> : ComEnumIteratorTraitsImpl<ATL::CComBSTR> {};
template <> struct ComEnumIteratorTraits<IEnumUnknown> : ComEnumIteratorTraitsImpl<ATL::CComPtr<IUnknown>> {};
template <> struct ComEnumIteratorTraits<IEnumMoniker> : ComEnumIteratorTraitsImpl<ATL::CComPtr<IMoniker>> {};

template<typename Fn>
struct deduce_item_traits {};


template <typename Ret, typename Itf, typename IndexType, typename ItemType>
struct deduce_item_traits<virtual COM_DECLSPEC_NOTHROW Ret(STDMETHODCALLTYPE Itf::*)(IndexType, ItemType*)>
{
    using index_type = IndexType;
    using item_type = ItemType;
};

template <typename Ret, typename Itf, typename ItemType>
struct deduce_item_traits<virtual COM_DECLSPEC_NOTHROW Ret(STDMETHODCALLTYPE Itf::*)(ItemType)>
{
    using item_type = ItemType;
    using index_type = std::false_type;
};

template<typename Fn>
struct ItemTraits
{
    using item_type = typename deduce_item_traits<Fn>::item_type;
    using index_type = typename deduce_item_traits<Fn>::index_type;
    ItemTraits(Fn fn) {};
};

template <typename T>
struct has_add
{
    struct S { void Add(); };
    struct Derived : S, T {};

    template <typename U, U>
    struct Helper {};

    using yes_type = char;
    using no_type = char(&)[2];

    template <typename Cont>
    static no_type test(Helper<decltype(&Cont::Add), &Cont::Add>*);

    template <typename Cont>
    static yes_type test(...);

    enum { value = sizeof(test<Derived>(nullptr)) == sizeof(yes_type) };
};

} // details


template <typename IEnum, typename Value = typename details::ComEnumIteratorTraits<IEnum>::value_type, typename Enable = void>
class ComEnumIterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Value;
    using difference_type = Value;
    using pointer = Value * ;
    using reference = Value & ;

    ComEnumIterator()
    {
    }

    explicit ComEnumIterator(IEnum* pIEnum) :
        m_pIEnum{ pIEnum }
    {
        Next();
    }

    Value& operator*()
    {
        return m_value;
    }

    Value* operator->()
    {
        return std::addressof(m_value);
    }

    ComEnumIterator& operator++()
    {
        Next();
        return *this;
    }

    ComEnumIterator operator++(int)
    {
        ComEnumIterator it(*this);
        ++*this;
        return it;
    }

    bool operator==(const ComEnumIterator& other) const
    {
        return m_pIEnum == other.m_pIEnum;
    }

    bool operator!=(const ComEnumIterator& other) const
    {
        return !(*this == other);
    }

private:
    void Next()
    {
        Value value;
        unsigned long count; // can pass nullptr but TEMServer doesn't always accept that..
        CheckedHResult chr = m_pIEnum->Next(1, &value, &count);
        if (chr.get() == S_FALSE)
        {
            m_pIEnum = nullptr;
        }
        else
        {
            m_value = value;
        }
    }

    ATL::CComPtr<IEnum> m_pIEnum;
    Value m_value;

};

template <typename Value>
class ComEnumIterator<IEnumVARIANT, Value, typename std::enable_if<!std::is_same<Value, ATL::CComVariant>::value>::type>
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Value;
    using difference_type = Value;
    using pointer = Value * ;
    using reference = Value & ;

    ComEnumIterator()
    {
    }

    explicit ComEnumIterator(IEnumVARIANT* pIEnum) :
        m_pIEnum{ pIEnum }
    {
        Next();
    }

    Value& operator*()
    {
        return m_value;
    }

    Value* operator->()
    {
        return std::addressof(m_value);
    }

    ComEnumIterator& operator++()
    {
        Next();
        return *this;
    }

    ComEnumIterator operator++(int)
    {
        ComEnumIterator it(*this);
        ++*this;
        return it;
    }

    bool operator==(const ComEnumIterator& other) const
    {
        return m_pIEnum == other.m_pIEnum;
    }

    bool operator!=(const ComEnumIterator& other) const
    {
        return !(*this == other);
    }

private:
    void Next()
    {
        ATL::CComVariant value;
        unsigned long count; // can pass nullptr but TEMServer doesn't always accept that..
        CheckedHResult chr = m_pIEnum->Next(1, &value, &count);
        if (chr.get() == S_FALSE)
        {
            m_pIEnum = nullptr;
        }
        else
        {
            m_value = variant_cast<Value>(value);
        }
    }

    ATL::CComPtr<IEnumVARIANT> m_pIEnum;
    Value m_value;
};

template <typename IColl, typename IndexType>
class ComCollectionItem
{
public:
    using item_type = typename details::ItemTraits<decltype(&IColl::get_Item)>::item_type;
    using value_type = typename ComWrapper<item_type>::type;

    explicit ComCollectionItem(ATL::CComPtr<IColl>& pIColl, IndexType index) :
        m_pIColl{ pIColl },
        m_index{ index }
    {
    }

    ComCollectionItem& operator=(item_type value)
    {
        CPP_COM_CALL(m_pIColl, put_Item)(m_index, value);
        return *this;
    }

    operator value_type() const
    {
        return CPP_COM_CALL(m_pIColl, get_Item)(m_index);
    }

private:
    ATL::CComPtr<IColl> m_pIColl;
    IndexType m_index;
};



template <typename Coll, typename IColl, typename Enable = void>
class ComCollectionPushBackImpl
{
};

template <typename Coll, typename IColl>
class ComCollectionPushBackImpl<Coll, IColl, typename std::enable_if<details::has_add<IColl>::value>::type>
{
public:
    using item_type = typename details::ItemTraits<decltype(&IColl::Add)>::item_type;

    void push_back(const item_type& v)
    {
        auto p = static_cast<Coll*>(this);
        ATL::CComPtr<IColl> pIColl = p->get();
        CPP_COM_CALL(pIColl, Add)(v);
    }
};



template <typename IColl, typename IEnum = IEnumVARIANT, typename Value = typename details::ComEnumIteratorTraits<IEnum>::value_type>
class ComCollection : public ComCollectionPushBackImpl<ComCollection<IColl, IEnum, Value>, IColl>
{
public:
    using value_type = Value;
    using const_reference = const Value&;
    using reference = Value & ;
    using const_pointer = const Value*;
    using pointer = Value * ;
    using size_type = size_t;
    using const_iterator = ComEnumIterator<IEnum, typename ComWrapper<Value>::type>;
    using iterator = const_iterator;

    explicit ComCollection(IColl* pIColl) :
        m_pIColl{ pIColl }
    {
    }

    explicit ComCollection(const ATL::CComPtr<IColl>& pIColl) :
        m_pIColl{ pIColl }
    {
    }

    ComCollection& operator=(const ATL::CComPtr<IColl>& pIColl)
    {
        m_IColl = pIColl;
        return *this;
    }


    ATL::CComPtr<IColl> get() const
    {
        return m_pIColl;
    }

    bool empty() const
    {
        return size() == 0;
    }

    size_type size() const
    {
        return CPP_COM_CALL(m_pIColl, get_Count)();
    }

    void clear()
    {
        return CPP_COM_CALL(m_pIColl, Clear)();
    }

    iterator begin() const
    {
        return iterator(GetInterface<IEnum>(CPP_COM_CALL(m_pIColl, get__NewEnum)()));
    }

    iterator end() const
    {
        return iterator();
    }

    using item_traits = typename details::ItemTraits<decltype(&IColl::get_Item)>;
    using index_type = typename item_traits::index_type;
    using item_type = typename item_traits::item_type;
    using com_wrapper_type = typename ComWrapper<item_type>::type;
    using item_value_type = ComCollectionItem<IColl, index_type>;

    com_wrapper_type operator[](index_type index) const
    {
        return CPP_COM_CALL(m_pIColl, get_Item)(index);
    }

    item_value_type operator[](index_type index)
    {
        return ComCollectionItem<IColl, index_type>(m_pIColl, index);
    }

    com_wrapper_type front() const
    {
        assert(!empty());
        return (*this)[0];
    }

    item_value_type front()
    {
        assert(!empty());
        return (*this)[0];
    }

    com_wrapper_type back() const
    {
        assert(!empty());
        auto index = CPP_COM_CALL(m_pIColl, get_Count)();
        return (*this)[index - 1];
    }

    item_value_type back()
    {
        assert(!empty());
        auto index = CPP_COM_CALL(m_pIColl, get_Count)();
        return (*this)[index - 1];
    }

private:
    ATL::CComPtr<IColl> m_pIColl;
};

} // com
} // cpp

