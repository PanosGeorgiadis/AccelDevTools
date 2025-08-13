#pragma once

#include "CeosProtocol/ServerCommandContext.h"

namespace Server {
namespace CeosProtocol {

class ServerCommandHandlerItf
{
public:
    virtual ~ServerCommandHandlerItf() {};
    virtual void HandleCommand(const std::shared_ptr<ServerCommandContext>& pContext) = 0;
};

} // namespace Ceos
} // namespace Server
