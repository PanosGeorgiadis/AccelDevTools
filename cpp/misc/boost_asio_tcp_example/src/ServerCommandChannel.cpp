#include <functional>
#include <stdint.h>
#include <boost/asio.hpp>
#include <inc/scope_guard.h>
#include "CeosProtocol/Constants.h"
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/ServerCommandChannel.h"
#include "AppLoggingConst.h"

namespace Server {
namespace CeosProtocol {

ServerCommandChannel::ServerCommandChannel(std::unique_ptr<MessageConnectionItf> pConnection, uint32_t connectionId, const CommandProtocolInfo& protocolInfo, const Logger& logger) :
    m_pConnection(std::move(pConnection)),
    m_connectionId(connectionId),
    m_isHandshakeCompleted(false),
    m_protocolInfo(protocolInfo),
    m_logger(logger),
    m_errorHandler([this] (const boost::system::error_code& error) { ReceiveError(error); })
{
    StartCheckMagicNumberHandshake();
}

ServerCommandChannel::~ServerCommandChannel()
{
    Disconnect();
}

void ServerCommandChannel::Disconnect()
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"ServerCommandChannel()::Disconnect()";
    m_pConnection = nullptr;
}

void ServerCommandChannel::Send(const EventMessage& message)
{
    if (!IsConnected())
        throw std::runtime_error("Not connected");
    if (!IsHandshakeCompleted())
        throw std::runtime_error("Channel not ready with handshake yet");

    for (auto eventChannelIt = m_eventChannels.begin(); eventChannelIt != m_eventChannels.end(); ++eventChannelIt)
        (*eventChannelIt)->Send(message);
}

void ServerCommandChannel::Send(const ResultMessage& message) const
{
    if (!IsConnected())
        throw std::runtime_error("Not connected");
    if (!IsHandshakeCompleted())
        throw std::runtime_error("Channel not ready with handshake yet");

    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"ServerCommandChannel(" << m_connectionId << L")::Send(Nr=" << message.Number() << L" ResultCode=" << message.ResultCode() << ")";
    m_pConnection->SendMessagePacket(message);
}

bool ServerCommandChannel::IsHandshakeCompleted() const
{
    return m_isHandshakeCompleted;
}

bool ServerCommandChannel::IsConnected() const
{
    return m_pConnection != nullptr && m_pConnection->IsConnected();
}

uint32_t ServerCommandChannel::ConnectionId() const
{
    return m_connectionId;
}

void ServerCommandChannel::AddEventChannel(const std::shared_ptr<ServerEventChannel>& pEventChannel)
{
    m_eventChannels.push_back(pEventChannel);
}

void ServerCommandChannel::CheckEventChannelsState()
{
    m_eventChannels.erase(std::remove_if(m_eventChannels.begin(), m_eventChannels.end(), [this] (const std::shared_ptr<ServerEventChannel>& pChannel) { return !pChannel->IsConnected(); }), m_eventChannels.end());
}

boost::signals2::connection ServerCommandChannel::ConnectCommandReceivedSignal(CommandReceivedSignal::slot_type slot)
{
    return m_signalCommandReceivedSignal.connect(slot);
}

void ServerCommandChannel::ReceiveError(const boost::system::error_code& error) const
{
    m_logger.Log(Infra::sevWarning, CAT_COMMUNICATION) << L"ServerCommandChannel(" << m_connectionId << L")::ReceiveError() - Error " << error;
}

void ServerCommandChannel::StartCheckMagicNumberHandshake()
{
    m_pConnection->StartReceiveRawPacket([this] (uint32_t value) { EndCheckMagicNumberHandshake(value); }, m_errorHandler);
}

void ServerCommandChannel::EndCheckMagicNumberHandshake(uint32_t magicNumber)
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"ServerCommandChannel(" << m_connectionId << L")::EndCheckMagicNumberHandshake() - MagicNumber " << magicNumber;

    if (magicNumber == MAGIC_NUMBER)
    {
        m_pConnection->SendRawPacket(MAGIC_NUMBER_SUCCESS);
        StartCheckVersionHandshake();
    }
    else if (magicNumber == SwapBytes(MAGIC_NUMBER))
    {
        m_pConnection->SendRawPacket(MAGIC_NUMBER_WRONG_BYTE_ORDER);
        Disconnect();
    }
    else
    {
        m_pConnection->SendRawPacket(MAGIC_NUMBER_CONNECTION_REFUSED);
        Disconnect();
    }
}

void ServerCommandChannel::StartCheckVersionHandshake()
{
    m_pConnection->StartReceiveRawPacket([this] (uint32_t value) { EndCheckVersionHandshake(value); }, m_errorHandler);
}

void ServerCommandChannel::EndCheckVersionHandshake(uint32_t majorProtocolVersion)
{
    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"ServerCommandChannel(" << m_connectionId << L")::EndCheckVersionHandshake() - MajorProtocolVersion " << majorProtocolVersion;

    if (majorProtocolVersion == MAJORNUMBER)
    {
        m_pConnection->SendRawPacket(PROTOCOL_OK);
        m_pConnection->SendRawPacket(m_connectionId);
        StartCheckLogon();
    }
    else
    {
        m_pConnection->SendRawPacket(WRONG_PROTOCOL_VERSION);
        m_pConnection->SendRawPacket(MAJORNUMBER);
        Disconnect();
    }
}

void ServerCommandChannel::StartCheckLogon()
{
    m_pConnection->StartReceiveCommand([this] (std::shared_ptr<CommandMessage> pCommand) { EndCheckLogon(pCommand); }, m_errorHandler);
}

void ServerCommandChannel::EndCheckLogon(const std::shared_ptr<CommandMessage>& pCommand)
{
    auto guard = make_guard([this] () { Disconnect(); });

    auto clientType = pCommand->GetDataBlock<uint32_t>(0)->Get(0);
    auto minorProtocolNumber = pCommand->GetDataBlock<uint32_t>(1)->Get(0);
    auto backwardCompatibilityNumber = pCommand->GetDataBlock<uint32_t>(2)->Get(0);
    auto clientId = pCommand->GetDataBlock<char>(3)->GetString();

    m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"ServerCommandChannel(" << m_connectionId << L")::EndCheckLogon() - clientType=" << clientType << L" Minor=" << minorProtocolNumber << L"Backward" << backwardCompatibilityNumber << L"ClientId" << clientId;
    
    uint32_t resultCode;
    if (clientType != m_protocolInfo.clientType)
        resultCode = RESULT_ERROR_UNKNOWN_CLIENT_TYPE;
    else if (minorProtocolNumber != m_protocolInfo.minorNumber || backwardCompatibilityNumber > m_protocolInfo.backwardNumber)
        resultCode = RESULT_ERROR_DIFFERENT_PROTOCOL_VERSION;
    else
        resultCode = RESULT_OK;

    auto result = ResultMessage(pCommand->Number(), resultCode);
    result.AddDataBlock<uint32_t>(L"")->AddValue(m_protocolInfo.minorNumber);
    result.AddDataBlock<uint32_t>(L"")->AddValue(m_protocolInfo.backwardNumber);
    m_pConnection->SendMessagePacket(result);

    if (resultCode == RESULT_OK)
    {
        StartReceiveCommand();
        m_isHandshakeCompleted = true;
        guard.release();
    }
}


void ServerCommandChannel::StartReceiveCommand()
{
    m_pConnection->StartReceiveCommand([this] (std::shared_ptr<CommandMessage> pCommand) { EndReceiveCommand(pCommand); }, m_errorHandler);
}

void ServerCommandChannel::EndReceiveCommand(const std::shared_ptr<CommandMessage>& pCommand)
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"ServerCommandChannel(" << m_connectionId << L")::EndReceiveCommand() - Nr=" << pCommand->Number() << L" OpCode=" << pCommand->OpCode();
    m_signalCommandReceivedSignal(pCommand);
    StartReceiveCommand();
}

} // namespace CeosProtocol
} // namespace Server
