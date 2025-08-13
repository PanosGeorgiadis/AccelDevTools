#include <boost/lexical_cast.hpp>
#include "inc/scope_guard.h"
#include "CeosProtocol/Constants.h"
#include "CeosProtocol/SocketConnection.h"
#include "CeosProtocol/Message.h" 
#include "CeosProtocol/AsynchronousResultHandler.h"

namespace Server {
namespace CeosProtocol {

SocketConnection::SocketConnection(std::unique_ptr<boost::asio::ip::tcp::socket> socket) :
    m_socket(std::move(socket))
{
}

SocketConnection::~SocketConnection()
{
    Disconnect();
}

bool SocketConnection::IsConnected() const
{
    return m_socket != nullptr;
}

void SocketConnection::Send(const RawData& data)
{
    if (!IsConnected())
        throw std::runtime_error("Sending data while not connected");

    auto guard = make_guard([this] () { Disconnect(); });
    m_socket->write_some(boost::asio::buffer(data.Data(), data.ByteSize()));
    guard.release();
}

boost::system::error_code SocketConnection::SendNoThrow(const RawData& data)
{
    if (!IsConnected())
        return boost::system::error_code(boost::system::errc::connection_aborted, boost::system::system_category());

    auto guard = make_guard([this] () { Disconnect(); });
    boost::system::error_code error;

    m_socket->write_some(boost::asio::buffer(data.Data(), data.ByteSize()), error);

    if (!error)
        guard.release();

    return error;
}

RawData SocketConnection::Receive(uint32_t byteSize)
{
    if (!IsConnected())
        throw std::runtime_error("Reading data while not connected");

     auto guard = make_guard([this] () { Disconnect(); });

    RawData data(byteSize);
    size_t readBytes = 0U;
    while (readBytes < data.ByteSize())
    {
        auto currentReadBytes = m_socket->read_some(boost::asio::buffer(data.Data(readBytes), data.ByteSize() - readBytes));
        readBytes += currentReadBytes;
    }

    guard.release();

    return data;
}

void SocketConnection::StartReceive(uint32_t byteSize, const std::function<void(const std::shared_ptr<RawData>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler)
{
    if (!IsConnected())
        throw std::runtime_error("Reading data while not connected");
    
    auto pRawData = std::make_shared<RawData>(byteSize);
    auto pHandler = std::make_shared<AsynchronousResultHandler>(pRawData, handler, errorHandler);
    StartReceive(0U, pHandler);
}

void SocketConnection::Disconnect()
{
    if (m_socket != nullptr)
    {
        boost::system::error_code errorCode;
        m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
        m_socket->close(errorCode);
        m_socket = nullptr;
    }
}

void SocketConnection::StartReceive(uint32_t readBytes, const std::shared_ptr<AsynchronousResultHandler>& pHandler)
{
    auto callback = [this, readBytes, pHandler] (const boost::system::error_code& error, std::size_t bytesTransferred) { EndReceive(pHandler, error, readBytes, bytesTransferred); };
    m_socket->async_read_some(boost::asio::buffer(pHandler->Data().Data(readBytes), pHandler->Data().ByteSize() - readBytes), callback);
}

void SocketConnection::EndReceive(const std::shared_ptr<AsynchronousResultHandler>& pHandler, const boost::system::error_code& error, uint32_t previousReadBytes, std::size_t bytesTransferred)
{
    if (!IsConnected())
        return;

    if (error)
    {
        Disconnect();
        pHandler->HandleError(error);
    }
    else
    {
        auto readBytes = previousReadBytes + static_cast<uint32_t>(bytesTransferred);
        
        if (static_cast<std::size_t>(readBytes) < pHandler->Data().ByteSize())
            StartReceive(readBytes, pHandler);
        else
            pHandler->HandleData();
    }
}

} // namespace CeosProtocol
} // namespace Server
