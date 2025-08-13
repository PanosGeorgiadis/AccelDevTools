#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "Generic/noncopyable.h"

namespace Server {
namespace CeosProtocol {

class RawData :
    public Infra::Generic::NonCopyable
{
public:
    explicit RawData(uint32_t maxLength) :
        m_data(maxLength),
        m_pStart(m_data.data()),
        m_pData(m_pStart)
    {
    }

    RawData(const RawData& other) : 
        m_data(other.m_data),
        m_pStart(m_data.data()),
        m_pData(m_pStart)
    {
        Advance<unsigned char>(other.AdvancedByteSize());
    }

    RawData(RawData&& other) : 
        m_data(std::move(other.m_data)),
        m_pStart(other.m_pStart),
        m_pData(other.m_pData)
    {
        other.m_pStart = nullptr;
        other.m_pData = nullptr;
    }

    RawData& operator=(const RawData& other)
    {
        if (&other!=this)
        {
            m_data = other.m_data;
            m_pStart = m_data.data();
            m_pData = m_pStart;
            Advance<unsigned char>(other.AdvancedByteSize());
        }
        return *this;
    }

    RawData& operator=(RawData&& other)
    {
        if (&other!=this)
        {
            m_data = std::move(other.m_data);
            m_pStart = other.m_pStart;
            m_pData = other.m_pData;
            other.m_pStart = nullptr;
            other.m_pData = nullptr;
        }
        return *this;
    }

public:
    template <typename T>
    T Read() const
    {
        CheckAdvance<T>();
        T value = *(reinterpret_cast<T*>(m_pData));
        Advance<T>();
        return value;
    }

    std::wstring Read(size_t length) const
    {
        std::wstring result;
        for (size_t i = 0; i < length; ++i)
            result.push_back(Read<char>());
        return result;
    }

    template <typename T>
    void Write(T value)
    {
        CheckAdvance<T>();
        *reinterpret_cast<T*>(m_pData) = value;
        Advance<T>();
    }

    template <>
    void Write(std::wstring value)
    {
        for (size_t i = 0; i < value.length(); ++i)
            Write<char>(static_cast<char>(value[i]));
    }

    void Reset() const
    {
        m_pData = m_pStart;
    }

    void* Data(size_t byteIndex=0) const
    {
        return reinterpret_cast<char*>(m_pStart) + byteIndex;
    }

    size_t ByteSize() const
    {
        return m_data.size();
    }

private:
    template <typename T>
    void Advance() const
    {
        Advance<T>(1U);
    }

    template <typename T>
    void Advance(size_t count) const
    {
        m_pData = reinterpret_cast<T*>(m_pData) + count;
    }

    template <typename T>
    void CheckAdvance() const
    {
        CheckAdvance<T>(1);
    }

    template <typename T>
    void CheckAdvance(size_t count) const
    {
        auto targetSize = AdvancedByteSize() + sizeof(T) * count;
        if (targetSize > ByteSize())
            throw std::out_of_range("Advanced to far in rawdata");
    }

    size_t AdvancedByteSize() const
    {
        return static_cast<unsigned char*>(m_pData) - static_cast<unsigned char*>(m_pStart);
    }
private:
    std::vector<unsigned char> m_data;
    void* m_pStart;
    mutable void* m_pData;
};

} // namespace Ceos
} // namespace Server
