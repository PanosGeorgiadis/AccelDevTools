#include "CeosProtocol/Message.h"
#include "CeosProtocol/RawData.h"

namespace Server {
namespace CeosProtocol {

Message::Message(uint32_t messageType, uint32_t messageNumber) :
    m_messageType(messageType),
    m_messageNumber(messageNumber)
{
}

Message::Message(const RawData& rawData)
{
    m_messageType = rawData.Read<uint32_t>();
    m_messageNumber = rawData.Read<uint32_t>();
}

Message::~Message()
{
}

uint32_t Message::Type() const
{
    return m_messageType;
}

uint32_t Message::Number() const
{
    return m_messageNumber;
}

uint32_t Message::ByteSize() const
{
    return 2 * sizeof(uint32_t) + GetDataSize();
}

std::wstring Message::Dump() const
{
    wstringbuilder builder;
    builder << L"Size:                 " << std::dec << ByteSize() << L"\r\n";
    builder << L"Type:                 " << std::dec << Type() << L"\r\n";
    builder << L"Number:               " << std::dec << Number() << L"\r\n";
    builder << DumpData();
    return builder.str();
}

bool Message::HasDatablock(uint32_t index, DataBlockType::type dataType)
{
    return m_pDataBlocks.size() > index && m_pDataBlocks[index]->DataType() == dataType; 
}

const std::vector<std::shared_ptr<DataBlockBase>>& Message::DataBlocks() const
{
    return m_pDataBlocks;
}

void Message::WriteTo(RawData& rawData) const
{
    rawData.Write(Type());
    rawData.Write(Number());
    WriteDataTo(rawData);
}

std::wstring Message::DumpDataBlocks() const
{
    wstringbuilder builder;
    builder << L"NumberOfDataBlocks:   " << std::dec << m_pDataBlocks.size() << L"\r\n";
    
    if (m_pDataBlocks.size() != 0)
    {
        builder << L"DataBlocks:" << L"\r\n";
        for (auto it = m_pDataBlocks.begin(); it != m_pDataBlocks.end(); ++it)
        {
            builder << L" Block Number:        " << it - m_pDataBlocks.begin() << L"\r\n";
            builder << L" Block Type:          " << enum_cast<std::wstring>((*it)->DataType()) << L"\r\n";
            builder << L" Block Identifier:    " << (*it)->Identifier() << L"\r\n";
            builder << (*it)->DumpData();
            builder << L"\r\n";
        }
    }
    return builder.str();
}

uint32_t Message::GetDataBlockSize() const
{
    uint32_t size = 1 * sizeof(uint32_t);

    for (auto it = m_pDataBlocks.begin(); it != m_pDataBlocks.end(); ++it)
        size += (*it)->ByteSize();

    return size;
}

void Message::ReadDataBlocks(const RawData& rawData)
{
    auto numberOfDataBlocks = rawData.Read<uint32_t>();
    for (uint32_t i = 0; i < numberOfDataBlocks; ++i)
        m_pDataBlocks.push_back(ReadDataBlock(rawData));
}

void Message::WriteDataBlocksTo(RawData& rawData) const
{
    rawData.Write(static_cast<uint32_t>(m_pDataBlocks.size()));
    for (auto it = m_pDataBlocks.begin(); it != m_pDataBlocks.end(); ++it)
        (*it)->Data(rawData);
}

} // namespace CeosProtocol
} // namespace Server
