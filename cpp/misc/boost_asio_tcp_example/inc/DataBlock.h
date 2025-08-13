#pragma once

#include <vector>
#include <string>
#include "CeosProtocol/DataBlockType.h"
#include <memory>

namespace Server {
namespace CeosProtocol {

class RawData;

class DataBlockBase
{
public:
    virtual ~DataBlockBase();
    virtual std::wstring Identifier() const = 0;
    virtual uint32_t ByteSize() const = 0;
    virtual void Data(RawData& rawData) const = 0;
    virtual std::wstring DumpData() const = 0;
    virtual DataBlockType::type DataType() const = 0;
};

template <typename T>
class DataBlock : public DataBlockBase
{
public:
    DataBlock(DataBlockType::type dataType, const std::wstring& identifier);
    DataBlock(DataBlockType::type dataType, const RawData& rawData);
public:
    std::wstring Identifier() const override;
    uint32_t ByteSize() const override;
    void Data(RawData& rawData) const override;
    std::wstring DumpData() const override;
    DataBlockType::type DataType() const override;

public:
    void AddValue(T value)
    {
        m_values.push_back(value);
    }

    void SetString(const std::wstring& value);
    std::wstring GetString();

public:
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    size_t size() const;
    bool empty() const;

    T operator[](size_t index) const
    {
        return Get(index);
    }

    T Get(size_t index) const
    {
        if (m_values.size() <= index)
            throw std::out_of_range(stringbuilder() << "Data index " << index << " in collection of " << m_values.size());
        return m_values[index];
    }
private:
    DataBlockType::type m_dataType;
    std::wstring m_identifier;
    std::vector<T> m_values;
};

std::shared_ptr<DataBlockBase> ReadDataBlock(const RawData& rawData);

template <typename T>
std::shared_ptr<DataBlock<T>> CreateDataBlock(const std::wstring& identifier);

template <typename T>
std::shared_ptr<DataBlock<T>> datablock_cast(const std::shared_ptr<DataBlockBase>& dataBlockBase)
{
    auto pDataBlock = std::dynamic_pointer_cast<DataBlock<T>>(dataBlockBase);
    if (pDataBlock == nullptr)
    {
#if _MSC_VER >= 1900
        throw std::bad_cast::__construct_from_string_literal("Wrong type");
#else
        throw std::bad_cast("Wrong type");
#endif
    }
    return pDataBlock;
}

} // namespace Ceos
} // namespace Server
