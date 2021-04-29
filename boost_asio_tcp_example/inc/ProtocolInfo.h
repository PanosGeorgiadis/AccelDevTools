#pragma once

#include <stdint.h>

namespace Server {
namespace CeosProtocol {

struct CommandProtocolInfo 
{
    CommandProtocolInfo(int commandPort, uint32_t clientType, uint32_t minorNumber, uint32_t backwardNumber) :
        commandPort(commandPort), clientType(clientType), minorNumber(minorNumber), backwardNumber(backwardNumber)
    {
    }
    int commandPort;
    uint32_t clientType;
    uint32_t minorNumber;
    uint32_t backwardNumber;
};

struct CommandEventProtocolInfo 
{
    CommandEventProtocolInfo(int commandPort, int eventPort, uint32_t clientType, uint32_t minorNumber, uint32_t backwardNumber) :
        commandPort(commandPort), eventPort(eventPort), clientType(clientType), minorNumber(minorNumber), backwardNumber(backwardNumber)
    {
    }
    int commandPort;
    int eventPort;
    uint32_t clientType;
    uint32_t minorNumber;
    uint32_t backwardNumber;
};

} // namespace Ceos
} // namespace Server
