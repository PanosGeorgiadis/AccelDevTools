#pragma once

#include "CeosProtocol/Message.h"

namespace Server {
namespace CeosProtocol {

class EventMessage : public Message
{
public:
    EventMessage(uint32_t commandMessageNumber, uint32_t eventType, const std::wstring& identifier);
    explicit EventMessage(const RawData& rawData);

public:
    uint32_t CommandMessageNumber() const;
    uint32_t EventType() const;
    std::wstring Identifier() const;

protected:
    uint32_t GetDataSize() const override;
    void WriteDataTo(RawData& rawData) const override;
    std::wstring DumpData() const override;

private:
    uint32_t m_commandMessageNumber;
    uint32_t m_eventType;
    std::wstring m_identifier;
};

} // namespace Ceos
} // namespace Server
