#pragma once

#include <stdint.h>
#include "CeosProtocol/MessageConnectionItf.h"
#include "CeosProtocol/CommandMessage.h"
#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/EventMessage.h"
#include "CeosProtocol/ServerEventChannel.h"
#include "CeosProtocol/Logger.h"
#include "CeosProtocol/ProtocolInfo.h"

class DispatcherItf;
namespace Server {
namespace CeosProtocol {
    
class ServerCommandChannel
{
public:
    ServerCommandChannel(std::unique_ptr<MessageConnectionItf> pConnection, uint32_t connectionId, const CommandProtocolInfo& protocolInfo, const Logger& logger=NullLogger());
    ~ServerCommandChannel();

    void Disconnect();

    void Send(const EventMessage& message);
    void Send(const ResultMessage& message) const;

    bool IsHandshakeCompleted() const;
    bool IsConnected() const;
    uint32_t ConnectionId() const;

    void AddEventChannel(const std::shared_ptr<ServerEventChannel>& pEventChannel);
    void CheckEventChannelsState();

    typedef boost::signals2::signal<void (const std::shared_ptr<CommandMessage>&)> CommandReceivedSignal;
    virtual boost::signals2::connection ConnectCommandReceivedSignal(CommandReceivedSignal::slot_type slot);
private:
    void ReceiveError(const boost::system::error_code& error) const;

    void StartCheckMagicNumberHandshake();
    void EndCheckMagicNumberHandshake(uint32_t value);

    void StartCheckVersionHandshake();
    void EndCheckVersionHandshake(uint32_t value);

    void StartCheckLogon();
    void EndCheckLogon(const std::shared_ptr<CommandMessage>& pMessage);

    void StartReceiveCommand();
    void EndReceiveCommand(const std::shared_ptr<CommandMessage>& pMessage);

    std::unique_ptr<MessageConnectionItf> m_pConnection;
    const uint32_t m_connectionId;
    bool m_isHandshakeCompleted;
    CommandProtocolInfo m_protocolInfo;
    std::vector<std::shared_ptr<ServerEventChannel>> m_eventChannels;

    CommandReceivedSignal m_signalCommandReceivedSignal;
    const Logger& m_logger;

    std::function<void(const boost::system::error_code& error)> m_errorHandler;
};

} // namespace Ceos
} // namespace Server
