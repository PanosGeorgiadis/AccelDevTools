#pragma once

#include "CeosProtocol/ResultMessage.h"
#include "CeosProtocol/EventMessage.h"

namespace Server {
namespace CeosProtocol {
    
class ClientEventHandlerItf
{
public:
    virtual ~ClientEventHandlerItf() {};
    virtual void HandleConnectedChanged(bool /*connected*/) = 0;
    virtual void HandleResult(const std::shared_ptr<ResultMessage>& /*pResult*/) = 0;
    virtual void HandleEvent(const std::shared_ptr<EventMessage>& /*pEvent*/) = 0;
};

} // namespace Ceos
} // namespace Server
