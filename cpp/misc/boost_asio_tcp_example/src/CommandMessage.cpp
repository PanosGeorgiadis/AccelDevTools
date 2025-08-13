#include "CeosProtocol/Constants.h"
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/RawData.h"
#include "MessageCounters.h"

namespace Server {
namespace CeosProtocol {

CommandMessage::CommandMessage(uint32_t command) :
    Message(MESSAGE_TYPE_COMMAND, MessageCounters::Instance().GetNextCommandMessageNumber()),
    m_opCode(command)
{
}

CommandMessage::CommandMessage(const RawData& rawData) :
    Message(rawData),
    m_opCode(rawData.Read<uint32_t>())
{
    if (Type() != MESSAGE_TYPE_COMMAND)
        throw std::runtime_error(stringbuilder() << "Incorrect type received" << Type());
    ReadDataBlocks(rawData);
}

uint32_t CommandMessage::OpCode() const
{
    return m_opCode;
}

uint32_t CommandMessage::GetDataSize() const
{
    return 1 * sizeof(uint32_t) + GetDataBlockSize();
}

void CommandMessage::WriteDataTo(RawData& rawData) const
{
    rawData.Write(OpCode());
    WriteDataBlocksTo(rawData);
}

std::wstring CommandMessage::DumpData() const
{
    wstringbuilder builder;
    builder << "OpCode:               " << std::dec << OpCode() << L"\r\n";
    builder << DumpDataBlocks();
    return builder.str();
}

} // namespace CeosProtocol
} // namespace Server
