#pragma once

#pragma comment(lib, "CeosProtocolLib.lib")

#include <boost/asio.hpp>
#include "CeosProtocol/ConnectionFactoryItf.h"

namespace Server {
namespace CeosProtocol {

class SocketConnectionFactory : public ConnectionFactoryItf
{
public:
    SocketConnectionFactory(const std::wstring& host, int port);
    std::unique_ptr<MessageConnectionItf> Connect(boost::asio::io_service& service) override;

private:
    const std::wstring m_host;
    const int m_port;
};

} // namespace Ceos
} // namespace Server
