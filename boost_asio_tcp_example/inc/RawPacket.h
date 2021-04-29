#pragma once

#include <stdint.h>
#include "CeosProtocol/Packet.h"

namespace Server {
namespace CeosProtocol {

class RawData;

class RawPacket : public Packet
{
public:
    explicit RawPacket(uint32_t content);
    explicit RawPacket(const RawData& rawData);

    uint32_t ByteSize() const override;
    void WriteTo(RawData& data) const override;
    std::wstring Dump() const override;
    
    uint32_t Content() const;
private:
    uint32_t m_content;
};

} // namespace Ceos
} // namespace Server
