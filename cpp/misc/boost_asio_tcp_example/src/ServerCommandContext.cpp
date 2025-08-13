#include <inc/DispatcherItf.h>
#include "CeosProtocol/ServerCommandChannel.h"
#include "CeosProtocol/ServerCommandContext.h"

namespace Server {
namespace CeosProtocol {

ServerCommandContext::ServerCommandContext(DispatcherItf& sendDispatcher, const std::shared_ptr<CommandMessage>& pCommand, const std::weak_ptr<ServerCommandChannel>& pCommandChannel) :
    m_sendDispatcher(sendDispatcher),
    m_pCommand(pCommand),
    m_pCommandChannel(pCommandChannel)
{
}

const CommandMessage& ServerCommandContext::Command() const
{
    return *m_pCommand;
}

void ServerCommandContext::Send(const EventMessage& message)
{
    m_sendDispatcher.Call([this, message] ()
    {
        auto pCommandChannel = m_pCommandChannel.lock();
        if (pCommandChannel == nullptr)
            throw std::runtime_error("Channel not connected");

        pCommandChannel->Send(message);
    });
}

void ServerCommandContext::Send(const ResultMessage& message)
{
    m_sendDispatcher.Call([this, message] ()
    {
        auto pCommandChannel = m_pCommandChannel.lock();
        if (pCommandChannel == nullptr)
            throw std::runtime_error("Channel not connected");

        pCommandChannel->Send(message);
    });
}

} // namespace CeosProtocol
} // namespace Server
