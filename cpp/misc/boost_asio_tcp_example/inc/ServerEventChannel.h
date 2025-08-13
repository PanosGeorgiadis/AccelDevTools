#pragma once

#include <stdint.h>
#include "Utilities/boost/signals2.hpp"
#include "CeosProtocol/EventMessage.h"
#include "CeosProtocol/MessageConnectionItf.h"
#include "CeosProtocol/Logger.h"

namespace Server {
namespace CeosProtocol {

class RawPacket;

class ServerEventChannel
{
public:
    ServerEventChannel(std::unique_ptr<MessageConnectionItf> pConnection, const Logger& logger=NullLogger());
    ~ServerEventChannel();

    void Send(const EventMessage& message) const;
    void SendConfirmation() const;
    void SendRejection() const;
    
    uint32_t ConnectionId() const;
    bool HasReceivedConnectionId() const;
    bool IsConnected() const;

    typedef boost::signals2::signal<void (int connectionId)> ConnectionIdChangedSignal;
    virtual boost::signals2::connection ConnectConnectionIdChanged(ConnectionIdChangedSignal::slot_type slot);

private:
    void ReceiveConnectionId(uint32_t connectionId);
    void ReceiveError(const boost::system::error_code& error);

    bool m_hasReceivedConnectionId;
    uint32_t m_connectionId;
    std::unique_ptr<MessageConnectionItf> m_pConnection;

    ConnectionIdChangedSignal m_signalConnectionIdChanged;
    const Logger& m_logger;
};

} // namespace Ceos
} // namespace Server
