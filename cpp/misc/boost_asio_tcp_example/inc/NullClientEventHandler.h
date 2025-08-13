#pragma once

#include "CeosProtocol/ClientEventHandlerItf.h"

namespace Server {
namespace CeosProtocol {

class NullClientEventHandler : public ClientEventHandlerItf
{
public:
    static std::shared_ptr<NullClientEventHandler> Create() { return std::make_shared<NullClientEventHandler>(); };
    virtual void HandleConnectedChanged(bool /*connected*/) {};
    virtual void HandleResult(const std::shared_ptr<ResultMessage>& /*pResult*/) {};
    virtual void HandleEvent(const std::shared_ptr<EventMessage>& /*pEvent*/) {};
};

} // namespace Ceos
} // namespace Server
