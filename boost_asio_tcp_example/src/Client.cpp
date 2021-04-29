#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <chrono>
#include "inc/scope_guard.h"
#include "inc/string_cast.h"
#include "CeosProtocol/Constants.h"
#include "CeosProtocol/SocketConnectionFactory.h"
#include "CeosProtocol/Client.h"
#include "AppLoggingConst.h"

namespace Server {
namespace CeosProtocol {
    
const auto PollInterval = std::chrono::milliseconds(1);

Client::Client(const std::wstring& clientName, const std::wstring& host, const CommandProtocolInfo& protocolInfo, const std::shared_ptr<ClientEventHandlerItf>& pEventHandler, const Logger& logger) :
    m_pCommandConnectionFactory(std::make_shared<SocketConnectionFactory>(host, protocolInfo.commandPort)),
    m_clientType(protocolInfo.clientType),
    m_minorNumber(protocolInfo.minorNumber),
    m_backwardNumber(protocolInfo.backwardNumber),
    m_clientName(clientName),
    m_connectionId(0),
    m_logger(logger),
    m_pollLoop(m_dispatcher.CallEvery(PollInterval, [this] () { Poll(); })),
    m_pEventHandler(pEventHandler)
{
    if (m_pEventHandler == nullptr)
        throw std::invalid_argument("No event handler");
    Connect();
}

Client::Client(const std::wstring& clientName, const std::wstring& host, const CommandEventProtocolInfo& protocolInfo, const std::shared_ptr<ClientEventHandlerItf>& pEventHandler, const Logger& logger) :
    m_pCommandConnectionFactory(std::make_shared<SocketConnectionFactory>(host, protocolInfo.commandPort)),
    m_pEventConnectionFactory(std::make_shared<SocketConnectionFactory>(host, protocolInfo.eventPort)),
    m_clientType(protocolInfo.clientType),
    m_minorNumber(protocolInfo.minorNumber),
    m_backwardNumber(protocolInfo.backwardNumber),
    m_clientName(clientName),
    m_connectionId(0),
    m_logger(logger),
    m_pollLoop(m_dispatcher.CallEvery(PollInterval, [this] () { Poll(); })),
    m_pEventHandler(pEventHandler)
{
    if (m_pEventHandler == nullptr)
        throw std::invalid_argument("No event handler");
    Connect();
}

Client::Client(const std::wstring& clientName, std::shared_ptr<ConnectionFactoryItf> pCommandConnectionFactory, const CommandProtocolInfo& protocolInfo, const std::shared_ptr<ClientEventHandlerItf>& pEventHandler, const Logger& logger) :
    m_pCommandConnectionFactory(pCommandConnectionFactory),
    m_clientType(protocolInfo.clientType),
    m_minorNumber(protocolInfo.minorNumber),
    m_backwardNumber(protocolInfo.backwardNumber),
    m_clientName(clientName),
    m_connectionId(0),
    m_logger(logger),
    m_pollLoop(m_dispatcher.CallEvery(PollInterval, [this] () { Poll(); })),
    m_pEventHandler(pEventHandler)
{
    if (pCommandConnectionFactory == nullptr)
        throw std::invalid_argument("No command connection factory");
    if (m_pEventHandler == nullptr)
        throw std::invalid_argument("No event handler");
    Connect();
}

Client::Client(const std::wstring& clientName, std::shared_ptr<ConnectionFactoryItf> pCommandConnectionFactory, std::shared_ptr<ConnectionFactoryItf> pEventConnectionFactory, const CommandEventProtocolInfo& protocolInfo, const std::shared_ptr<ClientEventHandlerItf>& pEventHandler, const Logger& logger) :
    m_pCommandConnectionFactory(pCommandConnectionFactory),
    m_pEventConnectionFactory(pEventConnectionFactory),
    m_clientType(protocolInfo.clientType),
    m_minorNumber(protocolInfo.minorNumber),
    m_backwardNumber(protocolInfo.backwardNumber),
    m_clientName(clientName),
    m_connectionId(0),
    m_logger(logger),
    m_pollLoop(m_dispatcher.CallEvery(PollInterval, [this] () { Poll(); })),
    m_pEventHandler(pEventHandler)
{
    if (pCommandConnectionFactory == nullptr)
        throw std::invalid_argument("No command connection factory");
    if (m_pEventConnectionFactory == nullptr)
        throw std::invalid_argument("No event connection factory");
    if (m_pEventHandler == nullptr)
        throw std::invalid_argument("No event handler");
    Connect();
}

Client::~Client()
{
    Disconnect();
}

bool Client::IsConnected() const
{
    return m_dispatcher.Call([this] () { return IsCommandChannelConnected() && (!HasEvents() || IsEventChannelConnected()); });
}

void Client::Connect()
{
    m_dispatcher.Call([this] () { ConnectInternal(); });
}

void Client::Disconnect()
{
    m_dispatcher.Call([this] () { DisconnectInternal(); });
    m_dispatcher.Synchronize();
}

std::shared_ptr<ResultMessage> Client::Send(const CommandMessage& message)
{
    if (!IsConnected())
        throw std::runtime_error("Not connected");

    auto guard = make_guard([this] () { Disconnect(); });

    m_dispatcher.Call([this, message] () { SendCommand(message); });
    auto pResult = ReceiveResult();
    EndReceiveResult(message, *pResult);
    
    guard.release();
    return pResult;
}

void Client::StartSend(const CommandMessage& message)
{
    if (!IsConnected())
        throw std::runtime_error("Not connected");

    m_dispatcher.Notify([this, message] () { StartSendInternal(message); });
}

void Client::ConnectInternal()
{
    assert(m_dispatcher.IsDispatcherThread());
    auto guard = make_guard([this]() { DisconnectInternal(); });
    
    ConnectCommandConnection();
    DoMagicNumberHandshake();
    DoProtocolVersionNumberHandshake();
    DoConnectionIdHandshake();
    
    Logon();

    if (HasEvents())
    {
        ConnectEventConnection();
        DoEventHandshake();
        StartEventReading();
    }
    
    m_pEventHandler->HandleConnectedChanged(true);

    guard.release();
}

void Client::DisconnectInternal()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client(" << m_connectionId << L")::Disconnect()";
    assert(m_dispatcher.IsDispatcherThread());
    m_pollLoop.Cancel();
    m_pCommandConnection = nullptr;
    m_pEventConnection = nullptr;
    m_pEventHandler->HandleConnectedChanged(false);
}

void Client::StartSendInternal(const CommandMessage& message)
try
{
    auto guard = make_guard([this] () { DisconnectInternal(); });

    SendCommand(message);
    auto pReply = m_pCommandConnection->ReceiveResult();
    EndReceiveResult(message, *pReply);
    
    guard.release();

    m_pEventHandler->HandleResult(pReply);
}
catch(std::exception& ex)
{
    HandleErrorResult(message, string_cast<std::wstring>(ex.what()));
}
catch(...)
{
    HandleErrorResult(message, L"Unknown error");
}

void Client::SendCommand(const CommandMessage& message)
{
    if (!IsCommandChannelConnected())
        throw std::runtime_error("Not connected");

    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"Client(" << m_connectionId << L")::SendCommand(Nr=" << message.Number() << L" OpCode=" << message.OpCode() << ")";
    m_pCommandConnection->SendMessagePacket(message);
}

std::shared_ptr<ResultMessage> Client::ReceiveResult()
{
    auto pResult = m_dispatcher.Call([this] () { return m_pCommandConnection->StartReceiveResult(); });
    return pResult->Get();
}

void Client::EndReceiveResult(const CommandMessage& commandMessage, const ResultMessage& resultMessage)
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"Client(" << m_connectionId << L")::ReceiveResult(Nr=" << resultMessage.Number() << L" ResultCode=" << resultMessage.ResultCode() << L")";
    if (commandMessage.Number() != resultMessage.Number())
        throw std::runtime_error(stringbuilder() << "Result number " << resultMessage.Number() << " does not match command number " << commandMessage.Number());
}

void Client::ConnectCommandConnection()
{
    m_pCommandConnection = m_pCommandConnectionFactory->Connect(m_ioService);
}

void Client::DoMagicNumberHandshake()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client()::DoMagicNumberHandshake()";
    m_pCommandConnection->SendRawPacket(MAGIC_NUMBER);
    auto reply = m_pCommandConnection->ReceiveRawPacket();
    
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client()::DoMagicNumberHandshake() - Received " << reply;
    if (reply == MAGIC_NUMBER_WRONG_BYTE_ORDER)
        throw std::runtime_error("Wrong byte order");
    
    if (reply == MAGIC_NUMBER_CONNECTION_REFUSED)
        throw std::runtime_error("Connected refused");

    if (reply != MAGIC_NUMBER_CONFIRMATION_SUCCESS)
        throw std::runtime_error("Unknown error in magic number handshake");
}

void Client::DoProtocolVersionNumberHandshake()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client()::DoProtocolVersionNumberHandshake()";
    m_pCommandConnection->SendRawPacket(MAJORNUMBER);
    auto reply = m_pCommandConnection->ReceiveRawPacket();
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client()::DoProtocolVersionNumberHandshake() - Received " << reply;
    if (reply == WRONG_PROTOCOL_VERSION)
    {
        auto kernelMajorProtocolVersion = m_pCommandConnection->ReceiveRawPacket();
        throw std::runtime_error(stringbuilder() << "Wrong major protocol version " << kernelMajorProtocolVersion);
    }

    if (reply != PROTOCOL_OK)
        throw std::runtime_error("Unknown error in protocol number handshake");
}

void Client::DoConnectionIdHandshake()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client()::DoConnectionIdHandshake()";
    m_connectionId = m_pCommandConnection->ReceiveRawPacket();
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client()::DoConnectionIdHandshake() - Received " << m_connectionId;
}

void Client::Logon()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client(" << m_connectionId << L")::Logon()";
    CommandMessage command(COMMAND_LOGON);
    command.AddDataBlock<uint32_t>(L"")->AddValue(m_clientType);
    command.AddDataBlock<uint32_t>(L"")->AddValue(m_minorNumber);
    command.AddDataBlock<uint32_t>(L"")->AddValue(m_backwardNumber);
    command.AddDataBlock<char>(L"")->SetString(m_clientName);
    
    SendCommand(command);
    auto reply = m_pCommandConnection->ReceiveResult();
    EndReceiveResult(command, *reply);

    auto minorProtocolNumber = reply->GetDataBlock<uint32_t>(0);
    auto backwardCompatiblityNumber = reply->GetDataBlock<uint32_t>(1);
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << "Client(" << m_connectionId << ")::Logon() - Received Result:" << reply->ResultCode() << " Minor: " << minorProtocolNumber << " Backward:" << backwardCompatiblityNumber;

    if (reply->ResultCode() == RESULT_ERROR_UNKNOWN_CLIENT_TYPE)
        throw std::runtime_error("Logon failed: unknown client type");
}

void Client::ConnectEventConnection()
{
    m_pEventConnection = m_pEventConnectionFactory->Connect(m_ioService);
}

void Client::DoEventHandshake()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client(" << m_connectionId << L")::DoEventHandshake()";
    m_pEventConnection->SendRawPacket(m_connectionId);
    auto reply = m_pEventConnection->ReceiveRawPacket();
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"Client(" << m_connectionId << L")::DoEventHandshake() - Received " << reply;

    if (reply == CONNECTION_ID_WRONG)
        throw std::runtime_error("Event connection id is wrong");

    if (reply != CONNECTION_ID_CONFIRMATION_SUCCESS)
        throw std::runtime_error("Unknown error during event connection handshake");
}

void Client::StartEventReading()
{
    m_pEventConnection->StartReceiveEvent(    [this] (const std::shared_ptr<EventMessage>& pMessage) { EndEventReading(pMessage); }, 
                                            [this] (const boost::system::error_code& error) { EndEventReadingError(error); });
}

void Client::EndEventReading(const std::shared_ptr<EventMessage>& pMessage)
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"Client(" << m_connectionId << L")::EndEventReading() - Received Nr=" << pMessage->Number() << L" CmdNr=" << pMessage->CommandMessageNumber() << L" EventType" << pMessage->EventType();
    m_pEventHandler->HandleEvent(pMessage);
    StartEventReading();
}

void Client::EndEventReadingError(const boost::system::error_code& error)
{
    m_logger.Log(Infra::sevError, CAT_COMMUNICATION) << L"Client(" << m_connectionId << L")::EndEventReadingError() - Error " << error.value();
    DisconnectInternal();
}

bool Client::IsCommandChannelConnected() const
{
    return m_pCommandConnection != nullptr && m_pCommandConnection->IsConnected();
}

bool Client::IsEventChannelConnected() const
{
    return m_pEventConnection != nullptr && m_pEventConnection->IsConnected();
}

bool Client::HasEvents() const
{
    return m_pEventConnectionFactory != nullptr;
}

void Client::Poll()
{
    m_ioService.poll();
}

void Client::HandleErrorResult(const CommandMessage& message, const std::wstring& errorDescription)
{
    auto pResultMessage = std::make_shared<ResultMessage>(message.Number(), RESULT_ERROR_INTERNAL);
    pResultMessage->AddDataBlock<char>(L"Error")->SetString(errorDescription);
    m_pEventHandler->HandleResult(pResultMessage);
}

} // namespace CeosProtocol
} // namespace Server
