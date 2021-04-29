#include <stdint.h>
#include <boost/asio.hpp>
#include <inc/string_cast.h>
#include "CeosProtocol/Constants.h"
#include "CeosProtocol/ServerEventChannel.h"
#include "AppLoggingConst.h"

namespace Server {
namespace CeosProtocol {

ServerEventChannel::ServerEventChannel(std::unique_ptr<MessageConnectionItf> pConnection, const Logger& logger) :
    m_hasReceivedConnectionId(false),
    m_connectionId(0),
    m_pConnection(std::move(pConnection)),
    m_logger(logger)
{
    m_pConnection->StartReceiveRawPacket([this] (uint32_t value) { ReceiveConnectionId(value); },
                                         [this] (const boost::system::error_code& error) { ReceiveError(error); });
}

ServerEventChannel::~ServerEventChannel()
{
}

void ServerEventChannel::Send(const EventMessage& message) const
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"ServerEventChannel(" << m_connectionId << L")::Send(Nr=" << message.Number() << L" CmdNr=" << message.CommandMessageNumber() << L" EventType=" << message.EventType() << L")";
    auto error = m_pConnection->SendMessagePacketNoThrow(message);
    if (error)
        m_logger.Log(Infra::sevInfo, CAT_COMMUNICATION) << L"ServerEventChannel(" << m_connectionId << L")::Send() - Error " << string_cast<std::wstring>(error.message());
}

void ServerEventChannel::SendConfirmation() const
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"ServerEventChannel(" << m_connectionId << L")::SendConfirmation()";
    m_pConnection->SendRawPacket(CONNECTION_ID_CONFIRMATION_SUCCESS);
}

void ServerEventChannel::SendRejection() const
{
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"ServerEventChannel(" << m_connectionId << L")::SendRejection()";
    m_pConnection->SendRawPacket(CONNECTION_ID_WRONG);
}

bool ServerEventChannel::HasReceivedConnectionId() const
{
    return m_hasReceivedConnectionId;
}

bool ServerEventChannel::IsConnected() const
{
    return m_pConnection != nullptr && m_pConnection->IsConnected();
}

uint32_t ServerEventChannel::ConnectionId() const
{
    return m_connectionId;
}

void ServerEventChannel::ReceiveConnectionId(uint32_t connectionId)
{
    m_connectionId = connectionId;
    m_logger.Log(Infra::sevInfo, CAT_DEVELOP1) << L"ServerEventChannel()::SendRejection() - Received " << m_connectionId;
    m_hasReceivedConnectionId = true;
    m_signalConnectionIdChanged(m_connectionId);
}

void ServerEventChannel::ReceiveError(const boost::system::error_code& error)
{
    m_logger.Log(Infra::sevWarning, CAT_COMMUNICATION) << L"ServerEventChannel()::ReceiveError() - Error " << error;
    m_pConnection = nullptr;
}

boost::signals2::connection ServerEventChannel::ConnectConnectionIdChanged(ConnectionIdChangedSignal::slot_type slot)
{
    return m_signalConnectionIdChanged.connect(slot);
}

} // namespace CeosProtocol
} // namespace Server
