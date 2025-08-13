#pragma once

#include "Generic/noncopyable.h"
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/EventMessage.h"

class DispatcherItf;
namespace Server {
namespace CeosProtocol {

class ServerCommandChannel;

class ServerCommandContext : 
    public Infra::Generic::NonCopyable
{
public:
    ServerCommandContext(DispatcherItf& sendDispatcher, const std::shared_ptr<CommandMessage>& pCommand, const std::weak_ptr<ServerCommandChannel>& pCommandChannel);
    
    const CommandMessage& Command() const;
    void Send(const EventMessage& message);
    void Send(const ResultMessage& message);

private:
    DispatcherItf& m_sendDispatcher;
    std::shared_ptr<CommandMessage> m_pCommand;
    std::weak_ptr<ServerCommandChannel> m_pCommandChannel;
};

} // namespace Ceos
} // namespace Server
