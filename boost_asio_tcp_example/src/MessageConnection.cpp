#include <inc/scope_guard.h>
#include "CeosProtocol/MessageConnection.h"
#include "CeosProtocol/RawPacket.h"

namespace Server {
namespace CeosProtocol {

RawData ToRawData(uint32_t content)
{
    auto packet = RawPacket(content);
    RawData data(packet.ByteSize());
    packet.WriteTo(data);
    return data;
}

RawData ToRawData(const Message& message)
{
    uint32_t messageSize = message.ByteSize() + sizeof(uint32_t);
    RawData data(messageSize);
    data.Write(messageSize);
    message.WriteTo(data);
    return data;
}

MessageConnection::MessageConnection(std::unique_ptr<ConnectionItf> pConnection) :
    m_pConnection(std::move(pConnection))
{
}

bool MessageConnection::IsConnected() const
{
    return m_pConnection->IsConnected();
}

uint32_t MessageConnection::ReceiveRawPacket()
{
    return RawPacket(m_pConnection->Receive(sizeof(uint32_t))).Content();
}

void MessageConnection::StartReceiveRawPacket(const std::function<void(uint32_t)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    auto rawPacketHandler = [this, handler] (const std::shared_ptr<RawData>& pRawData) { EndReceiveRawPacket(pRawData, handler); };
    m_pConnection->StartReceive(sizeof(uint32_t), rawPacketHandler, errorHandler);
}

void MessageConnection::SendRawPacket(uint32_t content)
{
    m_pConnection->Send(ToRawData(content));
}

boost::system::error_code MessageConnection::SendRawPacketNoThrow(uint32_t content)
{
    return m_pConnection->SendNoThrow(ToRawData(content));
}

std::shared_ptr<CommandMessage> MessageConnection::ReceiveCommand()
{
    return ReceiveMessage<CommandMessage>();
}

void MessageConnection::StartReceiveCommand(const std::function<void(const std::shared_ptr<CommandMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    StartReceiveMessage<CommandMessage>(handler, errorHandler);
}

std::shared_ptr<AsynchronousResult<CommandMessage>> MessageConnection::StartReceiveCommand()
{
    return StartReceiveMessage<CommandMessage>();
}

std::shared_ptr<ResultMessage> MessageConnection::ReceiveResult()
{
    return ReceiveMessage<ResultMessage>();
}

void MessageConnection::StartReceiveResult(const std::function<void(const std::shared_ptr<ResultMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    StartReceiveMessage<ResultMessage>(handler, errorHandler);
}

std::shared_ptr<AsynchronousResult<ResultMessage>> MessageConnection::StartReceiveResult()
{
    return StartReceiveMessage<ResultMessage>();
}

std::shared_ptr<EventMessage> MessageConnection::ReceiveEvent()
{
    return ReceiveMessage<EventMessage>();
}

void MessageConnection::StartReceiveEvent(const std::function<void(const std::shared_ptr<EventMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    StartReceiveMessage<EventMessage>(handler, errorHandler);
}

std::shared_ptr<AsynchronousResult<EventMessage>> MessageConnection::StartReceiveEvent()
{
    return StartReceiveMessage<EventMessage>();
}

void MessageConnection::SendMessagePacket(const Message& message)
{
    m_pConnection->Send(ToRawData(message));
}

boost::system::error_code MessageConnection::SendMessagePacketNoThrow(const Message& message)
{
    return m_pConnection->SendNoThrow(ToRawData(message));
}

void MessageConnection::EndReceiveRawPacket(const std::shared_ptr<RawData>& pRawData, const std::function<void(uint32_t)>& handler)
{
    handler(RawPacket(*pRawData).Content());
}

template <typename T>
std::shared_ptr<T> MessageConnection::ReceiveMessage()
{
    auto messageSize = ReceiveRawPacket();
    return std::make_shared<T>(m_pConnection->Receive(messageSize - sizeof(uint32_t)));
}

template <typename T>
void MessageConnection::StartReceiveMessage(const std::function<void(const std::shared_ptr<T>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    auto messageSizeHandler = [this, handler, errorHandler] (uint32_t messageSize) { EndReceiveMessageSize(messageSize, handler, errorHandler); };
    StartReceiveRawPacket(messageSizeHandler, errorHandler);
}

template <typename T> 
void MessageConnection::EndReceiveMessageSize(uint32_t messageSize, const std::function<void(const std::shared_ptr<T>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    auto messageContentHandler = [this, handler, errorHandler] (const std::shared_ptr<RawData>& pData) { EndReceiveMessageContent(pData, handler, errorHandler); };
    m_pConnection->StartReceive(messageSize - sizeof(uint32_t), messageContentHandler, errorHandler);
}

template <typename T> 
void MessageConnection::EndReceiveMessageContent(const std::shared_ptr<RawData>& pData, const std::function<void(const std::shared_ptr<T>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    auto guard = make_guard([errorHandler] () { errorHandler(boost::system::error_code(boost::system::errc::protocol_error, boost::system::system_category())); });
    auto pMessage = std::make_shared<T>(*pData);
    guard.release();
    handler(pMessage);
}

template <typename T>
std::shared_ptr<AsynchronousResult<T>> MessageConnection::StartReceiveMessage()
{
    auto pResult = std::make_shared<AsynchronousResult<T>>();
    auto handler = [pResult] (const std::shared_ptr<T>& pMessage) { pResult->HandleData(pMessage); };
    auto errorHandler = [pResult] (const boost::system::error_code& error) { pResult->HandleError(error); };
    StartReceiveMessage<T>(handler, errorHandler);
    return pResult;
}
} // namespace CeosProtocol
} // namespace Server
