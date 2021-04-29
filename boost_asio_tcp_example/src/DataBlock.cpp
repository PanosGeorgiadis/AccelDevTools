#include <iomanip>
#include "CeosProtocol/DataBlock.h"
#include "CeosProtocol/RawData.h"

namespace Server {
namespace CeosProtocol {

DataBlockBase::~DataBlockBase()
{
}

template <typename T>
DataBlock<T>::DataBlock(DataBlockType::type dataType, const std::wstring& identifier) :
    m_dataType(dataType),
    m_identifier(identifier)
{
}

template <typename T>
DataBlock<T>::DataBlock(DataBlockType::type dataType, const RawData& rawData) :
    m_dataType(dataType)
{
    uint32_t identitySize = rawData.Read<uint32_t>();
    if (identitySize > 0)
    {
        m_identifier = rawData.Read(identitySize - 1);
        rawData.Read<char>();
    }

    auto numberOfValues = rawData.Read<uint32_t>();
    for (unsigned int i = 0; i < numberOfValues; ++i)
        AddValue(rawData.Read<T>());
}

template <typename T>
std::wstring DataBlock<T>::Identifier() const
{
    return m_identifier;
}

template <typename T>
uint32_t DataBlock<T>::ByteSize() const
{
    uint32_t size = 2 * sizeof(uint32_t); // Datatype + identifier size 
    size += static_cast<uint32_t>(m_identifier.size()) * sizeof(char); // Identifier
    size += sizeof(char); // Identifier trailing 0
    size += sizeof(uint32_t); // number of items
    size += static_cast<uint32_t>(this->size()) * sizeof(T); // items

    return size;
}

template <typename T>
void DataBlock<T>::Data(RawData& rawData) const
{
    rawData.Write(enum_cast<unsigned int>(DataType()));
    rawData.Write<uint32_t>(static_cast<uint32_t>(m_identifier.size()) + 1U);
    rawData.Write(m_identifier);
    rawData.Write(static_cast<char>(0));

    rawData.Write<uint32_t>(static_cast<uint32_t>(size()));
    for (auto it = begin(); it != end(); ++it)
        rawData.Write(*it);
}

template <typename T>
std::wstring DataBlock<T>::DumpData() const
{
    wstringbuilder builder;
    builder << L"  Block Values:       " << size() << L"\r\n";
    for (auto it = begin(); it != end(); ++it)
    {
        builder << L"  Block Value Number: " << it - begin() << L"\r\n";
        builder << L"  Block Value:        " << *it << L"\r\n";
    }
    return builder.str();
}

template <>
std::wstring DataBlock<float>::DumpData() const
{
    wstringbuilder builder;
    builder << L"  Block Values:       " << size() << L"\r\n";
    for (auto it = begin(); it != end(); ++it)
    {
        builder << L"  Block Value Number: " << it - begin() << L"\r\n";
        builder << L"  Block Value:        " << std::setprecision(8) << *it << L"\r\n";
    }
    return builder.str();
}

template <>
std::wstring DataBlock<double>::DumpData() const
{
    wstringbuilder builder;
    builder << L"  Block Values:       " << size() << L"\r\n";
    for (auto it = begin(); it != end(); ++it)
    {
        builder << L"  Block Value Number: " << it - begin() << L"\r\n";
        builder << L"  Block Value:        " << std::setprecision(17) << *it << L"\r\n";
    }
    return builder.str();
}

template <typename T>
DataBlockType::type DataBlock<T>::DataType() const
{
    return m_dataType;
}

template <>
void DataBlock<char>::SetString(const std::wstring& value)
{
    for (auto it = value.begin(); it != value.end(); ++it)
        m_values.push_back(static_cast<char>(*it));

    m_values.push_back(0);
}

template <>
std::wstring DataBlock<char>::GetString()
{
    std::wstring value;
    size_t size = m_values.size();
    for (size_t i = 0; i < size; ++i)
        if (m_values[i] != 0)
            value.push_back(m_values[i]);

    return value;
}

template <typename T>
typename DataBlock<T>::iterator DataBlock<T>::begin()
{
    return m_values.begin();
}

template <typename T>
typename DataBlock<T>::const_iterator DataBlock<T>::begin() const
{
    return m_values.begin();
}

template <typename T>
typename DataBlock<T>::iterator DataBlock<T>::end()
{
    return m_values.end();
}

template <typename T>
typename DataBlock<T>::const_iterator DataBlock<T>::end() const
{
    return m_values.end();
}

template <typename T>
typename DataBlock<T>::reverse_iterator DataBlock<T>::rbegin()
{
    return m_values.rbegin();
}

template <typename T>
typename DataBlock<T>::const_reverse_iterator DataBlock<T>::rbegin() const
{
    return m_values.rbegin();
}

template <typename T>
typename DataBlock<T>::reverse_iterator DataBlock<T>::rend()
{
    return m_values.rend();
}

template <typename T>
typename DataBlock<T>::const_reverse_iterator DataBlock<T>::rend() const
{
    return m_values.rend();
}

template <typename T>
size_t DataBlock<T>::size() const
{
    return m_values.size();
}

template <typename T>
bool DataBlock<T>::empty() const
{
    return m_values.empty();
}

std::shared_ptr<DataBlockBase> ReadDataBlock(const RawData& rawData)
{
    auto dataType = enum_cast<DataBlockType::type>(rawData.Read<uint32_t>());
    switch (dataType)
    {
    case DataBlockType::Char:
        return std::make_shared<DataBlock<char>>(dataType, rawData);
    case DataBlockType::Float:
        return std::make_shared<DataBlock<float>>(dataType, rawData);
    case DataBlockType::Double:
        return std::make_shared<DataBlock<double>>(dataType, rawData);
    case DataBlockType::Integer:
        return std::make_shared<DataBlock<int>>(dataType, rawData);
    case DataBlockType::UnsignedInteger:
        return std::make_shared<DataBlock<unsigned int>>(dataType, rawData);
    case DataBlockType::Short:
        return std::make_shared<DataBlock<short>>(dataType, rawData);
    case DataBlockType::UnsignedShort:
        return std::make_shared<DataBlock<unsigned short>>(dataType, rawData);
    case DataBlockType::Bool:
        return std::make_shared<DataBlock<bool>>(dataType, rawData);
    default:
        throw std::runtime_error(stringbuilder() << "Invalid datatype " << dataType);
    }    
}

template <>
std::shared_ptr<DataBlock<char>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<char>>(DataBlockType::Char, identifier);
}

template <>
std::shared_ptr<DataBlock<float>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<float>>(DataBlockType::Float, identifier);
}

template <>
std::shared_ptr<DataBlock<double>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<double>>(DataBlockType::Double, identifier);
}

template <>
std::shared_ptr<DataBlock<int>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<int>>(DataBlockType::Integer, identifier);
}

template <>
std::shared_ptr<DataBlock<unsigned int>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<unsigned int>>(DataBlockType::UnsignedInteger, identifier);
}

template <>
std::shared_ptr<DataBlock<short>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<short>>(DataBlockType::Short, identifier);
}

template <>
std::shared_ptr<DataBlock<unsigned short>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<unsigned short>>(DataBlockType::UnsignedShort, identifier);
}

template <>
std::shared_ptr<DataBlock<bool>> CreateDataBlock(const std::wstring& identifier)
{
    return std::make_shared<DataBlock<bool>>(DataBlockType::Bool, identifier);
}

} // namespace CeosProtocol
} // namespace Server
