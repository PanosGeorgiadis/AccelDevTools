// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific
//
// CComSafeArray alternative:
// - default 1 dimension:    SafeArray<int> a(10); // int a[10];
// - multi dimensional type: SafeArray<int, 3> a(10, 11, 12); // int a[10][11][12];
// - multi dimensional indexing: a[3][4][5] = 42;
// - Direct memory access instead of SafeArrayGetElement()/SafeArrayPutElement() api for much better performance
// - SafeArrayRef<T, N> for non-copy reference access to existing SAFEARRAY.

#pragma once

#include <cassert>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/com/checked_hresult.h>
#include "details/iterator_traits.h"

namespace cpp
{
namespace com
{

template <typename T, unsigned Dimensions, unsigned Dim>
class SafeArrayIndexer
{
public:
    using return_type = SafeArrayIndexer<T, Dimensions, Dim - 1>;

    SafeArrayIndexer(const SAFEARRAY* psa, size_t index) :
        m_psa{ psa },
        m_index{ index }
    {
    }

    return_type operator[](size_t index)
    {
        return SafeArrayIndexer<T, Dimensions, Dim - 1>(m_psa, (m_index + index - m_psa->rgsabound[Dimensions - 1 - Dim].lLbound)*m_psa->rgsabound[Dimensions - Dim].cElements);
    }

private:
    const SAFEARRAY* m_psa;
    size_t m_index;
};

template <typename T, unsigned Dimensions>
class SafeArrayIndexer<T, Dimensions, 0>
{
public:
    using return_type = T & ;

    SafeArrayIndexer(const SAFEARRAY* psa, size_t index) :
        m_psa{ psa },
        m_index{ index }
    {
    }

    return_type operator[](size_t index)
    {
        return static_cast<T*>(m_psa->pvData)[m_index + index - m_psa->rgsabound[Dimensions - 1].lLbound];
    }

private:
    const SAFEARRAY* m_psa;
    size_t m_index;
};

template <typename T, unsigned Dimensions, unsigned Dim>
class ConstSafeArrayIndexer
{
public:
    using return_type = ConstSafeArrayIndexer<T, Dimensions, Dim - 1>;

    ConstSafeArrayIndexer(const SAFEARRAY* psa, size_t index) :
        m_psa{ psa },
        m_index{ index }
    {
    }

    return_type operator[](size_t index)
    {
        return ConstSafeArrayIndexer<T, Dimensions, Dim - 1>(m_psa, (m_index + index - m_psa->rgsabound[Dimensions - 1 - Dim].lLbound)*m_psa->rgsabound[Dimensions - Dim].cElements);
    }

private:
    const SAFEARRAY* m_psa;
    size_t m_index;
};

template <typename T, unsigned Dimensions>
class ConstSafeArrayIndexer<T, Dimensions, 0>
{
public:
    using return_type = const T&;

    ConstSafeArrayIndexer(const SAFEARRAY* psa, size_t index) :
        m_psa{ psa },
        m_index{ index }
    {
    }

    return_type operator[](size_t index)
    {
        return static_cast<const T*>(m_psa->pvData)[m_index + index - m_psa->rgsabound[Dimensions - 1].lLbound];
    }

private:
    const SAFEARRAY* m_psa;
    size_t m_index;
};

template <typename T, unsigned Dimensions = 1>
class SafeArrayRef
{
public:
    using value_type = typename ATL::_ATL_AutomationType<T>::_typewrapper;
    using const_reference = const typename ATL::_ATL_AutomationType<T>::_typewrapper&;
    using reference = typename ATL::_ATL_AutomationType<T>::_typewrapper&;
    using const_pointer = const typename ATL::_ATL_AutomationType<T>::_typewrapper*;
    using pointer = typename ATL::_ATL_AutomationType<T>::_typewrapper*;
    using size_type = unsigned;
#if _SECURE_SCL
    using const_iterator = stdext::checked_array_iterator<typename const ATL::_ATL_AutomationType<T>::_typewrapper*>;
    using iterator = stdext::checked_array_iterator<typename ATL::_ATL_AutomationType<T>::_typewrapper*>;
#else
    using const_iterator = typename const ATL::_ATL_AutomationType<T>::_typewrapper*;
    using iterator = typename ATL::_ATL_AutomationType<T>::_typewrapper*;
#endif

    explicit SafeArrayRef(SAFEARRAY* psa = nullptr) :
        m_psa{ psa }
    {
        if (psa && psa->cDims != Dimensions)
        {
            throw std::invalid_argument("SafeArray dimension mismatch");
        }

        if (!empty())
        {
            Lock();
        }
    }

    SafeArrayRef(const SafeArrayRef& sa) :
        m_psa(sa.m_psa)
    {
        if (!empty())
        {
            Lock();
        }
    }

    ~SafeArrayRef()
    {
        if (m_psa)
        {
            HRESULT hr = Unlock();
            static_cast<void>(hr);
            assert(SUCCEEDED(hr));
        }
    }

    void swap(SafeArrayRef& other)
    {
        std::swap(m_psa, other.m_psa);
    }

    using SafeArrayIndexer = SafeArrayIndexer<value_type, Dimensions, Dimensions - 1>;
    using const_SafeArrayIndexer = ConstSafeArrayIndexer<value_type, Dimensions, Dimensions - 1>;

    typename SafeArrayIndexer::return_type operator[](size_t index)
    {
        return SafeArrayIndexer(m_psa, 0)[index];
    }

    typename const_SafeArrayIndexer::return_type operator[](size_t index) const
    {
        return const_SafeArrayIndexer(m_psa, 0)[index];
    }

    bool empty() const
    {
        return !m_psa;
    }

    int size(unsigned dimension) const
    {
        if (dimension >= Dimensions)
        {
            throw std::invalid_argument("invalid SafeArray dimension");
        }

        if (!m_psa)
        {
            return 0;
        }

        return m_psa->rgsabound[Dimensions - 1 - dimension].cElements;
    }

    int size() const
    {
        if (!m_psa)
        {
            return 0;
        }

        int size = 1;

        for (unsigned dim = 0; dim < Dimensions; ++dim)
        {
            size *= m_psa->rgsabound[dim].cElements;
        }
        return size;
    }

    const_iterator begin() const
    {
#if _SECURE_SCL
        return const_iterator(static_cast<const_pointer>(m_psa ? m_psa->pvData : nullptr), size());
#else
        return static_cast<const_iterator>(m_psa ? m_psa->pvData : nullptr);
#endif
    }

    const_iterator end() const
    {
#if _SECURE_SCL
        return empty() ? begin() : begin() + size();
#else
        return begin() + size();
#endif
    }

    iterator begin()
    {
#if _SECURE_SCL
        return iterator(static_cast<pointer>(m_psa ? m_psa->pvData : nullptr), size());
#else
        return static_cast<iterator>(m_psa ? m_psa->pvData : nullptr);
#endif
    }

    iterator end()
    {
#if _SECURE_SCL
        return empty() ? begin() : begin() + size();
#else
        return begin() + size();
#endif
    }

    int GetLowerBound(unsigned int dimension = 0) const
    {
        if (dimension >= Dimensions)
        {
            throw std::invalid_argument("invalid SafeArray dimension");
        }

        return m_psa ? static_cast<int>(m_psa->rgsabound[Dimensions - 1 - dimension].lLbound) : 0;
    }

    int GetUpperBound(unsigned int dimension = 0) const
    {
        return GetLowerBound(dimension) + static_cast<int>(size(dimension)) - 1;
    }

    SAFEARRAY* get() const
    {
        return m_psa;
    }

    SAFEARRAY* release()
    {
        SAFEARRAY* psa = m_psa;
        if (m_psa)
        {
            CheckedHResult chr = Unlock();
            m_psa = nullptr;
        }
        return psa;
    }

    bool operator==(const SafeArrayRef& other) const
    {
        if (empty() || other.empty())
        {
            return empty() && other.empty();
        }

        for (unsigned i = 0; i < Dimensions; ++i)
        {
            if (m_psa->rgsabound[i].cElements != other.m_psa->rgsabound[i].cElements ||
                m_psa->rgsabound[i].lLbound != other.m_psa->rgsabound[i].lLbound)
                return false;
        }

        return std::equal(begin(), end(), other.begin());
    }

    bool operator!=(const SafeArrayRef& other) const
    {
        return !(*this == other);
    }

protected:
    SAFEARRAY * release_nothrow()
    {
        SAFEARRAY* psa = m_psa;
        if (m_psa)
        {
            HRESULT hr = Unlock();
            static_cast<void>(hr);
            assert(SUCCEEDED(hr));
            m_psa = nullptr;
        }
        return psa;
    }

private:
    void Lock()
    {
        CheckedHResult chr = SafeArrayLock(m_psa);
    }

    HRESULT Unlock()
    {
        return SafeArrayUnlock(m_psa);
    }

    SAFEARRAY* m_psa;
};

class SafeArrayWrapper; // ComWrapper.h

template <typename T, unsigned Dimensions = 1, VARTYPE VarType = ATL::_ATL_AutomationType<T>::type>
class SafeArray : public SafeArrayRef<T, Dimensions>
{
    using value_type = typename SafeArrayRef<T, Dimensions>::value_type;

public:
    SafeArray() :
        SafeArrayRef<T, 1>(nullptr)
    {
    }

    // generic overload for all N-dimensionsional arrays:
    explicit SafeArray(SAFEARRAYBOUND const (&bounds)[Dimensions]) :
        SafeArrayRef<T, Dimensions>(Create(bounds))
    {
    }

    // (size) overload for 1D arrays only:
    template <typename Unsigned>
    explicit SafeArray(Unsigned size, int lowerBound = 0,
                       typename std::enable_if<Dimensions == 1 && std::is_convertible<Unsigned, unsigned>::value>::type* = 0) :
        SafeArrayRef<T, Dimensions>(Create(size, lowerBound))
    {
    }

    // (rows, columns) overload for 2D arrays only:
    template <typename Unsigned>
    SafeArray(Unsigned rows, unsigned columns, int lowerRowBound = 0, int lowerColumnBound = 0,
              typename std::enable_if<Dimensions == 2 && std::is_convertible<Unsigned, unsigned>::value>::type* = 0) :
        SafeArrayRef<T, Dimensions>(Create(rows, columns, lowerRowBound, lowerColumnBound))
    {
    }

    // overload for 3D arrays only:
    template <typename Unsigned>
    SafeArray(Unsigned n2, unsigned n1, unsigned n0, int lBound2 = 0, int lBound1 = 0, int lBound0 = 0,
              typename std::enable_if<Dimensions == 3 && std::is_convertible<Unsigned, unsigned>::value>::type* = 0) :
        SafeArrayRef<T, Dimensions>(Create(n2, n1, n0, lBound2, lBound1, lBound0))
    {
    }

    // MIGRATION ISSUE: The implementation of std::iterator_traits is no longer
    // SFINAE-friendly, so we can't use it to rule out some instantiations of
    // member functions in SafeArray. A new, SFINAE-friendly type trait was
    // defined.
    template <typename FwdIterator>
    SafeArray(FwdIterator first, FwdIterator last, int lowerBound = 0,
              typename std::enable_if<Dimensions == 1 && std::is_convertible<typename traits::iterator_value_type<FwdIterator>::type, value_type>::value>::type* = 0) :
        SafeArrayRef<T, Dimensions>(Create(last - first, lowerBound))
    {
        std::copy(first, last, this->begin());
    }

    SafeArray(const SafeArray& other) :
        SafeArrayRef<T, Dimensions>(Create(other.get()))
    {
        std::copy(other.begin(), other.end(), this->begin());
    }

    SafeArray(SafeArray&& other) :
        SafeArrayRef<T, Dimensions>(other.get())
    {
        other.release();
    }

    template <typename U>
    SafeArray(const SafeArrayRef<U, Dimensions>& other,
              typename std::enable_if<std::is_convertible<U, value_type>::value>::type* = 0) :
        SafeArrayRef<T, Dimensions>(Create(other.get()))
    {
        std::copy(other.begin(), other.end(), this->begin());
    }

    SafeArray(SafeArrayWrapper&& other) :
        SafeArrayRef<T, Dimensions>(other.get())
    {
        other.detach();
    }

    ~SafeArray()
    {
        if (!this->empty())
        {
            HRESULT hr = SafeArrayDestroy(this->release_nothrow());
            static_cast<void>(hr);
            assert(SUCCEEDED(hr));
        }
    }

    SafeArray& operator=(SafeArray other)
    {
        this->swap(other);
        return *this;
    }

private:
    template <typename To, typename From>
    static To ConvertTo(From n)
    {
        if (n < (std::numeric_limits<To>::min)())
        {
            throw std::range_error("Value too small");
        }

        if (n > (std::numeric_limits<To>::max)())
        {
            throw std::range_error("Value too large");
        }

        return static_cast<To>(n);
    }

    static SAFEARRAY* Create(size_t size, ptrdiff_t lowerBound)
    {
        SAFEARRAYBOUND bounds[1] = { ConvertTo<unsigned long>(size), ConvertTo<long>(lowerBound) };
        return Create(bounds);
    }

    static SAFEARRAY* Create(size_t rows, size_t columns, ptrdiff_t lowerRowBound, ptrdiff_t lowerColumnBound)
    {
        SAFEARRAYBOUND bounds[2] =
        {
            { ConvertTo<unsigned long>(columns), ConvertTo<long>(lowerColumnBound) },
        { ConvertTo<unsigned long>(rows), ConvertTo<long>(lowerRowBound) }
        };
        return Create(bounds);
    }

    static SAFEARRAY* Create(size_t n2, size_t n1, size_t n0, ptrdiff_t lBound2, ptrdiff_t lBound1, ptrdiff_t lBound0)
    {
        SAFEARRAYBOUND bounds[3] =
        {
            { ConvertTo<unsigned long>(n0), ConvertTo<long>(lBound0) },
            { ConvertTo<unsigned long>(n1), ConvertTo<long>(lBound1) },
            { ConvertTo<unsigned long>(n2), ConvertTo<long>(lBound2) }
        };
        return Create(bounds);
    }

    static SAFEARRAY* Create(SAFEARRAYBOUND const (&bounds)[Dimensions])
    {
        SAFEARRAY* psa(SafeArrayCreate(VarType, Dimensions, const_cast<SAFEARRAYBOUND*>(bounds)));
        if (!psa)
        {
            throw std::bad_alloc();
        }

        return psa;
    }

    static SAFEARRAY* Create(const SAFEARRAY* psa)
    {
        if (psa->cDims != Dimensions)
        {
            throw std::invalid_argument("SAFEARRAY dimension mismatch");
        }

        SAFEARRAYBOUND bounds[Dimensions];
        for (unsigned i = 0; i < Dimensions; ++i)
        {
            bounds[i] = psa->rgsabound[Dimensions - 1 - i];
        }

        return Create(bounds);
    }
};

template <typename T, unsigned Dimensions>
HRESULT ComRetVal(const SafeArrayRef<T, Dimensions>& sa, SAFEARRAY** ppValue)
{
    return SafeArrayCopy(sa.get(), ppValue);
}

template <typename T, unsigned Dimensions>
HRESULT ComRetVal(SafeArray<T, Dimensions>&& sa, SAFEARRAY** ppValue)
{
    if (!ppValue)
    {
        return E_POINTER;
    }

    *ppValue = sa.release();
    return S_OK;
}

} // com 
} // cpp

