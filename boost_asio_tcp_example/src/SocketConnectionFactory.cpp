#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "Fei/string_cast.h"
#include "CeosProtocol/SocketConnection.h"
#include "CeosProtocol/MessageConnection.h"
#include "CeosProtocol/SocketConnectionFactory.h"

namespace Fei {
namespace Server {
namespace CeosProtocol {

SocketConnectionFactory::SocketConnectionFactory(const std::wstring& host, int port) :
    m_host(host),
    m_port(port)
{
}

std::unique_ptr<MessageConnectionItf> SocketConnectionFactory::Connect(boost::asio::io_service& service)
{
    boost::asio::ip::tcp::resolver resolver(service);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), string_cast<std::string>(m_host), boost::lexical_cast<std::string>(m_port));
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    
    auto pSocket = std::make_unique<boost::asio::ip::tcp::socket>(service);
    pSocket->connect(*iterator);

    return std::make_unique<MessageConnection>(std::make_unique<SocketConnection>(std::move(pSocket)));
}

} // namespace CeosProtocol
} // namespace Server
} // namespace Fei
