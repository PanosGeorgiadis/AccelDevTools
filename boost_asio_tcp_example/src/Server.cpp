#include <stdint.h>
#include <boost/asio.hpp>
#include <chrono>
#include "CeosProtocol/SocketConnection.h"
#include "CeosProtocol/ServerCommandChannel.h"
#include "CeosProtocol/ServerEventChannel.h"
#include "CeosProtocol/MessageConnection.h"
#include "CeosProtocol/Server.h"
#include "AppLoggingConst.h"

namespace Server {
namespace CeosProtocol {

const auto PollInterval = std::chrono::milliseconds(1);

Server::Server(const std::shared_ptr<ServerCommandHandlerItf>& pCommandHandler, const CommandProtocolInfo& protocolInfo, const Logger& logger) :
    m_pCommandHandler(pCommandHandler),
    m_clientType(protocolInfo.clientType),
    m_minorNumber(protocolInfo.minorNumber),
    m_backwardNumber(protocolInfo.backwardNumber),
    m_commandAcceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(protocolInfo.commandPort))),
    m_eventAcceptor(m_ioService),
    m_commandConnectionCounter(0),
    m_logger(logger),
    m_pollLoop(m_dispatcher.CallEvery(PollInterval, [this] () { Poll(); }))
{
    if (pCommandHandler == nullptr)
        throw std::invalid_argument("pCommandHandler == nullptr");
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Server()::Server() - Accepting commandConnections at " << protocolInfo.commandPort;
    m_dispatcher.Call([this] () { StartCommandAccept(); });
}

Server::Server(const std::shared_ptr<ServerCommandHandlerItf>& pCommandHandler, const CommandEventProtocolInfo& protocolInfo, const Logger& logger) :
    m_pCommandHandler(pCommandHandler),
    m_clientType(protocolInfo.clientType),
    m_minorNumber(protocolInfo.minorNumber),
    m_backwardNumber(protocolInfo.backwardNumber),
    m_commandAcceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(protocolInfo.commandPort))),
    m_eventAcceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), static_cast<unsigned short>(protocolInfo.eventPort))),
    m_commandConnectionCounter(0),
    m_logger(logger),
    m_pollLoop(m_dispatcher.CallEvery(PollInterval, [this] () { Poll(); }))
{
    if (pCommandHandler == nullptr)
        throw std::invalid_argument("pCommandHandler == nullptr");
    
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Server()::Server() - Accepting commandConnections at " << protocolInfo.commandPort << " and eventConnections at " << protocolInfo.eventPort;

    m_dispatcher.Call([this] () 
    { 
        StartCommandAccept(); 
        StartEventAccept();
    });
}

Server::~Server()
{
    m_dispatcher.Call([this] () { Disconnect(); });
    m_dispatcher.Synchronize();
}

void Server::Send(const EventMessage& message)
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"Server()::Send(Nr=" << message.Number() << L" CmdNr=" << message.CommandMessageNumber() << L" EventType=" << message.EventType() << L")";
    m_dispatcher.Call([this, message] () 
    {
        for (auto commandChannelIt = m_commandChannels.begin(); commandChannelIt != m_commandChannels.end(); ++commandChannelIt)
            if ((*commandChannelIt)->pChannel->IsConnected() && (*commandChannelIt)->pChannel->IsHandshakeCompleted())
                (*commandChannelIt)->pChannel->Send(message);
    });
}

void Server::StartCommandAccept()
{
    assert(m_dispatcher.IsDispatcherThread());

    auto endAccept = [this] (const boost::system::error_code& error) { EndCommandAccept(error); };
    m_pCommandSocket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioService);
    m_commandAcceptor.async_accept(*m_pCommandSocket, endAccept);
}

void Server::EndCommandAccept(const boost::system::error_code& error)
{
    assert(m_dispatcher.IsDispatcherThread());

    if (!error)
    {
        m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"Server()::EndCommandAccept() - Succes";
        CommandProtocolInfo protocolInfo(0, m_clientType, m_minorNumber, m_backwardNumber);
        auto pConnection = std::make_unique<MessageConnection>(std::make_unique<SocketConnection>(std::move(m_pCommandSocket)));
        auto pChannel = std::make_shared<ServerCommandChannel>(std::move(pConnection), m_commandConnectionCounter, protocolInfo, m_logger);
        auto pChannelWeak = std::weak_ptr<ServerCommandChannel>(pChannel);
        auto subscription = pChannel->ConnectCommandReceivedSignal([this, pChannelWeak](const std::shared_ptr<CommandMessage>& pCommand) { HandleCommand(pCommand, pChannelWeak); });
        m_commandChannels.push_back(std::make_unique<CommandChannelPair>(pChannel, subscription));
        m_commandConnectionCounter++;
        StartCommandAccept();
    }
    else
        m_logger.Log(Infra::sevError, CAT_COMMUNICATION) << L"Server()::EndCommandAccept() - Error " << error;
}

void Server::StartEventAccept()
{
    assert(m_dispatcher.IsDispatcherThread());

    auto endAccept = [this] (const boost::system::error_code& error) { EndEventAccept(error); };
    m_pEventSocket = std::make_unique<boost::asio::ip::tcp::socket>(m_ioService);
    m_eventAcceptor.async_accept(*m_pEventSocket, endAccept);
}


void Server::EndEventAccept(const boost::system::error_code& error)
{
    assert(m_dispatcher.IsDispatcherThread());

    if (!error)
    {
        m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"Server()::EndEventAccept() - Succes";
        auto pConnection = std::make_unique<MessageConnection>(std::make_unique<SocketConnection>(std::move(m_pEventSocket)));
        auto pChannel = std::make_shared<ServerEventChannel>(std::move(pConnection), m_logger);
        auto pChannelWeak = std::weak_ptr<ServerEventChannel>(pChannel);
        auto subscription = pChannel->ConnectConnectionIdChanged([this, pChannelWeak](int connectionId) { HandleEventConnectionId(connectionId, pChannelWeak); });
        m_eventChannels.push_back(std::make_unique<EventChannelPair>(pChannel, subscription));
        
        StartEventAccept();
    }
    else
        m_logger.Log(Infra::sevError, CAT_COMMUNICATION) << L"Server()::EndEventAccept() - Error " << error;
}

void Server::Poll()
{
    m_ioService.poll();
    CheckConnectedState();
}

void Server::CheckConnectedState()
{
    m_commandChannels.erase(std::remove_if(m_commandChannels.begin(), m_commandChannels.end(), [this] (const std::unique_ptr<CommandChannelPair>& pChannel) { return !pChannel->pChannel->IsConnected(); }), m_commandChannels.end());
    m_eventChannels.erase(std::remove_if(m_eventChannels.begin(), m_eventChannels.end(), [this] (const std::unique_ptr<EventChannelPair>& pChannel) { return !pChannel->pChannel->IsConnected(); }), m_eventChannels.end());

    for (auto channelIt = m_commandChannels.begin(); channelIt != m_commandChannels.end(); ++channelIt)
        (*channelIt)->pChannel->CheckEventChannelsState();
}

void Server::HandleEventConnectionId(uint32_t connectionId, const std::weak_ptr<ServerEventChannel>& pChannelWeak)
{
    auto pChannel = pChannelWeak.lock();
    if (pChannel == nullptr)
        return;

    auto commandChannelIt = std::find_if(m_commandChannels.begin(), m_commandChannels.end(), 
        [this, connectionId] (const std::unique_ptr<CommandChannelPair>& pChannel) { return pChannel->pChannel->ConnectionId() == connectionId; });

    RemoveEventChannel(pChannel);
    if (commandChannelIt != m_commandChannels.end() && (*commandChannelIt)->pChannel->IsConnected() && (*commandChannelIt)->pChannel->IsHandshakeCompleted())
    {
        (*commandChannelIt)->pChannel->AddEventChannel(pChannel);
        pChannel->SendConfirmation();
    }
    else
    {
        pChannel->SendRejection();
    }
}

void Server::RemoveEventChannel(const std::shared_ptr<ServerEventChannel>& pChannel)
{
    m_eventChannels.erase(std::remove_if(m_eventChannels.begin(), m_eventChannels.end(), [this, pChannel] (const std::unique_ptr<EventChannelPair>& pair) { return pair->pChannel == pChannel; }), m_eventChannels.end());
}

void Server::HandleCommand(const std::shared_ptr<CommandMessage>& pCommand, const std::weak_ptr<ServerCommandChannel>& pChannel)
{
    auto context = std::make_shared<ServerCommandContext>(m_dispatcher, pCommand, pChannel);
    m_pCommandHandler->HandleCommand(context);
}

void Server::Disconnect()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Server()::Disconnect()";
    m_pollLoop.Cancel();
    m_commandAcceptor.close();
    m_eventAcceptor.close();
    m_commandChannels.clear();
    m_eventChannels.clear();
}

} // namespace CeosProtocol
} // namespace Server
