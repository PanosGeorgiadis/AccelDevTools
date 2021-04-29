#pragma once

#include <stdint.h>
#include <boost/asio.hpp>
#include "Generic/noncopyable.h"
#include "CeosProtocol/RawData.h"

namespace Server {
namespace CeosProtocol {

class ConnectionItf :
    public Infra::Generic::NonCopyable
{
public:
    virtual ~ConnectionItf() {};
    virtual bool IsConnected() const = 0;
    virtual void Send(const RawData& data) = 0;
    virtual boost::system::error_code SendNoThrow(const RawData& data) = 0;
    virtual RawData Receive(uint32_t byteSize) = 0;
    virtual void StartReceive(uint32_t byteSize, const std::function<void(const std::shared_ptr<RawData>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) = 0;
};

} // namespace Ceos
} // namespace Server
