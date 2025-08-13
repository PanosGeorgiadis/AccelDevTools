#pragma once

#include <stdint.h>
#include <boost/thread.hpp>

namespace Server {
namespace CeosProtocol {

class MessageCounters
{
public:
    static MessageCounters& Instance();
    MessageCounters();

    uint32_t GetNextCommandMessageNumber();
    uint32_t GetNextEventMessageNumber();

private:
    mutable boost::mutex m_lock;
    uint32_t m_commandMessageNumber;
    uint32_t m_eventMessageNumber;
};

} // namespace Ceos
} // namespace Server
