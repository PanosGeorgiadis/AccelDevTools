#include "CeosProtocol/Constants.h"
#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/RawData.h"
#include "MessageCounters.h"

namespace Server {
namespace CeosProtocol {

ResultMessage::ResultMessage(uint32_t commandMessageNumber, uint32_t result) :
    Message(MESSAGE_TYPE_RESULT, commandMessageNumber),
    m_resultCode(result)
{
}

ResultMessage::ResultMessage(const RawData& rawData) :
    Message(rawData),
    m_resultCode(rawData.Read<uint32_t>())
{
    if (Type() != MESSAGE_TYPE_RESULT)
        throw std::runtime_error(stringbuilder() << "Incorrect type received" << Type());

    ReadDataBlocks(rawData);
}

uint32_t ResultMessage::ResultCode() const
{
    return m_resultCode;
}

uint32_t ResultMessage::GetDataSize() const
{
    return 1 * sizeof(uint32_t) + GetDataBlockSize();
}

void ResultMessage::WriteDataTo(RawData& rawData) const
{
    rawData.Write(ResultCode());
    WriteDataBlocksTo(rawData);
}

std::wstring ResultMessage::DumpData() const
{
    wstringbuilder builder;
    builder << "ResultCode:           " << std::dec << ResultCode() << L"\r\n";
    builder << DumpDataBlocks();
    return builder.str();
}

} // namespace CeosProtocol
} // namespace Server
