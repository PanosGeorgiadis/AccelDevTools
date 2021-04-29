#pragma once

#include "CeosProtocol/Message.h"

namespace Server {
namespace CeosProtocol {

class ResultMessage : public Message
{
public:
    explicit ResultMessage(uint32_t commandMessageNumber, uint32_t result);
    explicit ResultMessage(const RawData& rawData);

public:
    uint32_t ResultCode() const;

protected:
    uint32_t GetDataSize() const override;
    void WriteDataTo(RawData& rawData) const override;
    std::wstring DumpData() const override;

private:
    uint32_t m_resultCode;
};

} // namespace Ceos
} // namespace Server
