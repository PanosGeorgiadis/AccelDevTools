#pragma once

#include <stdint.h>
#include "Generic/noncopyable.h"
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/EventMessage.h"
#include "CeosProtocol/AsynchronousResult.h"

namespace Server {
namespace CeosProtocol {

class MessageConnectionItf :
    public Infra::Generic::NonCopyable
{
public:
    virtual ~MessageConnectionItf() {};
    
    virtual bool IsConnected() const = 0;

    virtual uint32_t ReceiveRawPacket() = 0;
    virtual void StartReceiveRawPacket(const std::function<void(uint32_t)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) = 0;
    virtual void SendRawPacket(uint32_t content) = 0;
    virtual boost::system::error_code SendRawPacketNoThrow(uint32_t content) = 0;

    virtual std::shared_ptr<CommandMessage> ReceiveCommand() = 0;
    virtual void StartReceiveCommand(const std::function<void(const std::shared_ptr<CommandMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) = 0;
    virtual std::shared_ptr<AsynchronousResult<CommandMessage>> StartReceiveCommand() = 0;

    virtual std::shared_ptr<ResultMessage> ReceiveResult() = 0;
    virtual void StartReceiveResult(const std::function<void(const std::shared_ptr<ResultMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) = 0;
    virtual std::shared_ptr<AsynchronousResult<ResultMessage>> StartReceiveResult() = 0;

    virtual std::shared_ptr<EventMessage> ReceiveEvent() = 0;
    virtual void StartReceiveEvent(const std::function<void(const std::shared_ptr<EventMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) = 0;
    virtual std::shared_ptr<AsynchronousResult<EventMessage>> StartReceiveEvent() = 0;

    virtual void SendMessagePacket(const Message& message) = 0;
    virtual boost::system::error_code SendMessagePacketNoThrow(const Message& message) = 0;
};

} // namespace Ceos
} // namespace Server
