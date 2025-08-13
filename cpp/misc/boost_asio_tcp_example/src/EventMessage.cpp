#include "CeosProtocol/Constants.h"
#include "CeosProtocol/EventMessage.h"
#include "CeosProtocol/RawData.h"
#include "MessageCounters.h"

namespace Server {
namespace CeosProtocol {

EventMessage::EventMessage(uint32_t commandMessageNumber, uint32_t eventType, const std::wstring& identifier) : 
    Message(MESSAGE_TYPE_EVENT, MessageCounters::Instance().GetNextEventMessageNumber()),
    m_commandMessageNumber(commandMessageNumber),
    m_eventType(eventType),
    m_identifier(identifier)
{
}

EventMessage::EventMessage(const RawData& rawData) :
    Message(rawData),
    m_commandMessageNumber(rawData.Read<uint32_t>()),
    m_eventType(rawData.Read<uint32_t>())
{
    if (Type() != MESSAGE_TYPE_EVENT)
        throw std::runtime_error(stringbuilder() << "Incorrect type received" << Type());

    uint32_t identitySize = rawData.Read<uint32_t>();
    if (identitySize > 0)
    {
        m_identifier = rawData.Read(identitySize - 1);
        rawData.Read<char>();
    }

    ReadDataBlocks(rawData);
}

uint32_t EventMessage::CommandMessageNumber() const
{
    return m_commandMessageNumber;
}

uint32_t EventMessage::EventType() const
{
    return m_eventType;
}

std::wstring EventMessage::Identifier() const
{
    return m_identifier;
}

uint32_t EventMessage::GetDataSize() const
{
    uint32_t size = 3 * sizeof(uint32_t);
    size += static_cast<uint32_t>(m_identifier.size());
    size += sizeof(char);
    size += GetDataBlockSize();
    return size;
}

void EventMessage::WriteDataTo(RawData& rawData) const
{
    rawData.Write(CommandMessageNumber());
    rawData.Write(EventType());
    rawData.Write(static_cast<uint32_t>(m_identifier.size() + 1U));
    rawData.Write(m_identifier);
    rawData.Write(static_cast<char>(0));
    WriteDataBlocksTo(rawData);
}

std::wstring EventMessage::DumpData() const
{
    wstringbuilder builder;
    builder << L"CommandMessageNumber: " << std::dec << CommandMessageNumber() << L"\r\n";
    builder << L"EventType:            " << std::dec << EventType() << L"\r\n";
    builder << L"Identifier:           " << Identifier() << L"\r\n";
    builder << DumpDataBlocks();
    return builder.str();
}

} // namespace CeosProtocol
} // namespace Server
