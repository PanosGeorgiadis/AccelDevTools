#include "Fei/stringbuilder.h"
#include "CeosProtocol/RawData.h"
#include "CeosProtocol/RawPacket.h"

namespace Server {
namespace CeosProtocol {

RawPacket::RawPacket(uint32_t content) :
    m_content(content)
{
}

RawPacket::RawPacket(const RawData& rawData)
{
    m_content = rawData.Read<unsigned int>();
}

uint32_t RawPacket::ByteSize() const
{
    return sizeof(uint32_t);
}

void RawPacket::WriteTo(RawData& data) const
{
    data.Write(Content());
}

std::wstring RawPacket::Dump() const
{
    wstringbuilder builder;
    builder << L"Content:              " << std::dec << m_content << L"\r\n";
    return builder.str();
}

uint32_t RawPacket::Content() const
{
    return m_content;
}

} // namespace CeosProtocol
} // namespace Server
