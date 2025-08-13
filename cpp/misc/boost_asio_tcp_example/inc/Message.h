#pragma once

#include <vector>
#include <string>
#include "CeosProtocol/Packet.h"
#include "CeosProtocol/DataBlock.h"

namespace Server {
namespace CeosProtocol {

class Message : public Packet
{
public:
    Message(uint32_t messageType, uint32_t messageNumber);
    explicit Message(const RawData& rawData);
    virtual ~Message();

    uint32_t Type() const;
    uint32_t Number() const;

    uint32_t ByteSize() const override;
    void WriteTo(RawData& data) const override;
    std::wstring Dump() const override;
    
    const std::vector<std::shared_ptr<DataBlockBase>>& DataBlocks() const;
    template <typename T>
    std::shared_ptr<DataBlock<T>> AddDataBlock(const std::wstring& identifier)
    {
        auto pDataBlock = CreateDataBlock<T>(identifier);
        m_pDataBlocks.push_back(pDataBlock);
        return pDataBlock;
    }

    bool HasDatablock(uint32_t index, DataBlockType::type dataType);

    template <typename T>
    std::shared_ptr<DataBlock<T>> GetDataBlock(uint32_t index) const
    {
        if (m_pDataBlocks.size() <= index)
            throw std::out_of_range(stringbuilder() << "Datablocks index " << index << " in collection of " << m_pDataBlocks.size());

        return datablock_cast<T>(m_pDataBlocks[index]);
    }

protected:
    virtual uint32_t GetDataSize() const = 0;
    virtual void WriteDataTo(RawData& rawData) const = 0;
    virtual std::wstring DumpData() const = 0;
    
    std::wstring DumpDataBlocks() const;
    uint32_t GetDataBlockSize() const;
    void ReadDataBlocks(const RawData& rawData);
    void WriteDataBlocksTo(RawData& rawData) const;

private:
    uint32_t m_messageType;
    uint32_t m_messageNumber;
    std::vector<std::shared_ptr<DataBlockBase>> m_pDataBlocks;
};

} // namespace Ceos
} // namespace Server
