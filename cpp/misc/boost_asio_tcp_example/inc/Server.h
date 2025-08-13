#pragma once

#include <stdint.h>
#include <boost/asio.hpp>
#include "Utilities/boost/signals2.hpp"
#include <inc/Dispatcher.h>
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/EventMessage.h"
#include "CeosProtocol/ServerCommandHandlerItf.h"
#include "CeosProtocol/Logger.h"
#include "CeosProtocol/ProtocolInfo.h"

namespace Server {
namespace CeosProtocol {

class ServerCommandChannel;
class ServerEventChannel;

class Server
{
public:
    Server(const std::shared_ptr<ServerCommandHandlerItf>& pCommandHandler, const CommandProtocolInfo& protocolInfo, const Logger& logger=NullLogger());
    Server(const std::shared_ptr<ServerCommandHandlerItf>& pCommandHandler, const CommandEventProtocolInfo& protocolInfo, const Logger& logger=NullLogger());
    ~Server();

    void Send(const EventMessage& message);

private:
    void StartCommandAccept();
    void EndCommandAccept(const boost::system::error_code& error);
    void StartEventAccept();
    void EndEventAccept(const boost::system::error_code& error);

    void Poll();
    void CheckConnectedState();
    void HandleEventConnectionId(uint32_t connectionId, const std::weak_ptr<ServerEventChannel>& pConnection);
    void RemoveEventChannel(const std::shared_ptr<ServerEventChannel>& pConnection);

    void HandleCommand(const std::shared_ptr<CommandMessage>& pCommand, const std::weak_ptr<ServerCommandChannel>& pChannel);
    void Disconnect();

    std::shared_ptr<ServerCommandHandlerItf> m_pCommandHandler;

    uint32_t m_clientType;
    uint32_t m_minorNumber;
    uint32_t m_backwardNumber;

    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::acceptor m_commandAcceptor;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_pCommandSocket;
    boost::asio::ip::tcp::acceptor m_eventAcceptor;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_pEventSocket;

    uint32_t m_commandConnectionCounter;

    template <typename T>
    struct ChannelPair
    {
        ChannelPair(const std::shared_ptr<T>& pChannel, boost::signals2::connection& subscription) :
            pChannel(pChannel), subscription(subscription)
        {
        }

        std::shared_ptr<T> pChannel;
        boost::signals2::scoped_connection subscription;
    };
    typedef ChannelPair<ServerCommandChannel> CommandChannelPair;
    typedef ChannelPair<ServerEventChannel> EventChannelPair;
    std::vector<std::unique_ptr<CommandChannelPair>> m_commandChannels;
    std::vector<std::unique_ptr<EventChannelPair>> m_eventChannels;

    const Logger& m_logger;
    Dispatcher m_dispatcher;
    ScheduledCall m_pollLoop;
};

} // namespace Ceos
} // namespace Server
