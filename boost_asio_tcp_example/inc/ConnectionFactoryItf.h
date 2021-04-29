#pragma once

#include "Generic/noncopyable.h"
#include <boost/asio.hpp>
#include "CeosProtocol/MessageConnectionItf.h"

namespace Server {
namespace CeosProtocol {

class ConnectionFactoryItf :
    public Infra::Generic::NonCopyable
{
public:
    virtual ~ConnectionFactoryItf() {};
    virtual std::unique_ptr<MessageConnectionItf> Connect(boost::asio::io_service& service) = 0;
};

} // namespace Ceos
} // namespace Server
