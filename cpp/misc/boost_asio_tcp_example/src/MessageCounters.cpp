
#include "MessageCounters.h"

namespace Server {
namespace CeosProtocol {

MessageCounters& MessageCounters::Instance()
{
    static MessageCounters instance;
    return instance;
}

MessageCounters::MessageCounters() :
    m_commandMessageNumber(0),
    m_eventMessageNumber(0)
{
}

uint32_t MessageCounters::GetNextCommandMessageNumber()
{
    boost::mutex::scoped_lock lock(m_lock);
    ++m_commandMessageNumber;
    return m_commandMessageNumber;
}

uint32_t MessageCounters::GetNextEventMessageNumber()
{
    boost::mutex::scoped_lock lock(m_lock);
    ++m_eventMessageNumber;
    return m_eventMessageNumber;
}

} // namespace CeosProtocol
} // namespace Server
