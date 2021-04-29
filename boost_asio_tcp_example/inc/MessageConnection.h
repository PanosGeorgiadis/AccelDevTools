#pragma once

#include "CeosProtocol/ConnectionItf.h"
#include "CeosProtocol/MessageConnectionItf.h"

namespace Server {
namespace CeosProtocol {

class MessageConnection : public MessageConnectionItf
{
public:
    explicit MessageConnection(std::unique_ptr<ConnectionItf> pConnection);

    bool IsConnected() const override;

    uint32_t ReceiveRawPacket() override;
    void StartReceiveRawPacket(const std::function<void(uint32_t)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) override;
    void SendRawPacket(uint32_t content) override;
    boost::system::error_code SendRawPacketNoThrow(uint32_t content) override;

    std::shared_ptr<CommandMessage> ReceiveCommand() override;
    void StartReceiveCommand(const std::function<void(const std::shared_ptr<CommandMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) override;
    std::shared_ptr<AsynchronousResult<CommandMessage>> StartReceiveCommand() override;

    std::shared_ptr<ResultMessage> ReceiveResult() override;
    void StartReceiveResult(const std::function<void(const std::shared_ptr<ResultMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) override;
    std::shared_ptr<AsynchronousResult<ResultMessage>> StartReceiveResult() override;

    std::shared_ptr<EventMessage> ReceiveEvent() override;
    void StartReceiveEvent(const std::function<void(const std::shared_ptr<EventMessage>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) override;
    std::shared_ptr<AsynchronousResult<EventMessage>> StartReceiveEvent() override;

    void SendMessagePacket(const Message& message) override;
    boost::system::error_code SendMessagePacketNoThrow(const Message& message) override;
    
private:
    static void EndReceiveRawPacket(const std::shared_ptr<RawData>& pRawData, const std::function<void(uint32_t)>& handler);

    template <typename T> std::shared_ptr<T> ReceiveMessage();
    template <typename T> void StartReceiveMessage(const std::function<void(const std::shared_ptr<T>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler);
    template <typename T> void EndReceiveMessageSize(uint32_t messageSize, const std::function<void(const std::shared_ptr<T>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler);
    template <typename T> void EndReceiveMessageContent(const std::shared_ptr<RawData>& pData, const std::function<void(const std::shared_ptr<T>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler);
    template <typename T> std::shared_ptr<AsynchronousResult<T>> StartReceiveMessage();

    std::unique_ptr<ConnectionItf> m_pConnection;
};

} // namespace Ceos
} // namespace Server
