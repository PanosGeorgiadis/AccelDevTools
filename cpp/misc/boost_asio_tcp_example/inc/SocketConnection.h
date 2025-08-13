#pragma once

#include "CeosProtocol/ConnectionItf.h"

namespace Server {
namespace CeosProtocol {

class AsynchronousResultHandler;

class SocketConnection : public ConnectionItf
{
public:
    SocketConnection(std::unique_ptr<boost::asio::ip::tcp::socket> socket);
    ~SocketConnection();

    bool IsConnected() const override;
    void Send(const RawData& data) override;
    boost::system::error_code SendNoThrow(const RawData& data) override;
    RawData Receive(uint32_t byteSize) override;
    void StartReceive(uint32_t byteSize, const std::function<void(const std::shared_ptr<RawData>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) override;

private:
    void Disconnect();
    void StartReceive(uint32_t readBytes, const std::shared_ptr<AsynchronousResultHandler>& pHandler);
    void EndReceive(const std::shared_ptr<AsynchronousResultHandler>& pHandler, const boost::system::error_code& error, uint32_t previousReadBytes, std::size_t bytesTransferred);

    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
};

} // namespace Ceos
} // namespace Server
