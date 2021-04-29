#pragma once

#include <cstdint>

namespace Server {
namespace CeosProtocol {

class RawData;

class Packet
{
public:
    virtual ~Packet() {};
    virtual uint32_t ByteSize() const = 0;
    virtual void WriteTo(RawData& data) const = 0;
    virtual std::wstring Dump() const = 0;
};

} // namespace Ceos
} // namespace Server
