#pragma once

#include <stdint.h>
#include <boost/asio.hpp>
#include <inc/Dispatcher.h>
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/EventMessage.h"
#include "CeosProtocol/ClientEventHandlerItf.h"
#include "CeosProtocol/NullClientEventHandler.h"
#include "CeosProtocol/Logger.h"
#include "CeosProtocol/ProtocolInfo.h"

namespace Server {
namespace CeosProtocol {

class MessageConnectionItf;
class ConnectionFactoryItf;
class AsynchronousReadCall;
class ConnectionSupport;

class Client
{
public:
    Client(const std::wstring& clientName, const std::wstring& host, const CommandProtocolInfo& protocolInfo,
           const std::shared_ptr<ClientEventHandlerItf>& pEventHandler=NullClientEventHandler::Create(),
           const Logger& logger=NullLogger());
    Client(const std::wstring& clientName, const std::wstring& host, const CommandEventProtocolInfo& protocolInfo,
           const std::shared_ptr<ClientEventHandlerItf>& pEventHandler=NullClientEventHandler::Create(),
           const Logger& logger=NullLogger());
    Client(const std::wstring& clientName, std::shared_ptr<ConnectionFactoryItf> pCommandConnectionFactory,
           const CommandProtocolInfo& protocolInfo,
           const std::shared_ptr<ClientEventHandlerItf>& pEventHandler=NullClientEventHandler::Create(),
           const Logger& logger=NullLogger());
    Client(const std::wstring& clientName, std::shared_ptr<ConnectionFactoryItf> pCommandConnectionFactory,
           std::shared_ptr<ConnectionFactoryItf> pEventConnectionFactory,
           const CommandEventProtocolInfo& protocolInfo,
           const std::shared_ptr<ClientEventHandlerItf>& pEventHandler=NullClientEventHandler::Create(),
           const Logger& logger=NullLogger());
    ~Client();

    std::shared_ptr<ResultMessage> Send(const CommandMessage& message);
    void StartSend(const CommandMessage& message);
    bool IsConnected() const;
    
private:
    void Connect();
    void Disconnect();

    void ConnectInternal();
    void DisconnectInternal();

    void StartSendInternal(const CommandMessage& message);
    void SendCommand(const CommandMessage& message);
    std::shared_ptr<ResultMessage> ReceiveResult();
    void EndReceiveResult(const CommandMessage& command, const ResultMessage& result);
    
    void ConnectCommandConnection();
    void DoMagicNumberHandshake();
    void DoProtocolVersionNumberHandshake();
    void DoConnectionIdHandshake();
    
    void Logon();
    
    void ConnectEventConnection();
    void DoEventHandshake();
    
    void StartEventReading();
    void EndEventReading(const std::shared_ptr<EventMessage>& pMessage);
    void EndEventReadingError(const boost::system::error_code& error);

    bool IsCommandChannelConnected() const;
    bool IsEventChannelConnected() const;
    bool HasEvents() const;

    void Poll();

    void HandleErrorResult(const CommandMessage& message, const std::wstring& errorDescription);

    boost::asio::io_service m_ioService;

    std::shared_ptr<ConnectionFactoryItf> m_pCommandConnectionFactory;
    std::shared_ptr<ConnectionFactoryItf> m_pEventConnectionFactory;
    std::unique_ptr<MessageConnectionItf> m_pCommandConnection;
    std::unique_ptr<MessageConnectionItf> m_pEventConnection;

    uint32_t m_clientType;
    uint32_t m_minorNumber;
    uint32_t m_backwardNumber;
    const std::wstring m_clientName;
    const std::vector<uint32_t> m_eventIds;

    uint32_t m_connectionId;

    const Logger& m_logger;
    Dispatcher m_dispatcher;
    ScheduledCall m_pollLoop;

    std::shared_ptr<ClientEventHandlerItf> m_pEventHandler;
};

} // namespace Ceos
} // namespace Server
