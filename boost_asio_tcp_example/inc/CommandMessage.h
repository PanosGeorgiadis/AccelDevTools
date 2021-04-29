#pragma once

#include "CeosProtocol/Message.h"

namespace Server {
namespace CeosProtocol {

class CommandMessage : public Message
{
public:
    explicit CommandMessage(uint32_t command);
    explicit CommandMessage(const RawData& rawData);

public:
    uint32_t OpCode() const;

protected:
    uint32_t GetDataSize() const override;
    void WriteDataTo(RawData& rawData) const override;
    std::wstring DumpData() const override;

private:
    uint32_t m_opCode;
};

} // namespace Ceos
} // namespace Server
