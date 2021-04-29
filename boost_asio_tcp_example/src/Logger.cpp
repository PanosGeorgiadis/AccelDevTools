#include "CeosProtocol/Logger.h"

namespace Server {
namespace CeosProtocol {

LogEntry::LogEntry(const Logger& logger, long severity, long category) :
    m_logger(logger), m_hasOriginator(false), m_severity(severity), m_category(category)
{
}

LogEntry::LogEntry(const Logger& logger, const std::wstring& originator, long severity, long category) :
    m_logger(logger), m_hasOriginator(true), m_originator(originator), m_severity(severity), m_category(category)
{
}

LogEntry::~LogEntry()
{
    if (m_hasOriginator)
        m_logger.Log(m_originator, m_severity, m_category, m_ss.str());
    else
        m_logger.Log(m_severity, m_category, m_ss.str());
}

const Logger& NullLogger()
{
    const static NullLogger_impl instance;
    return instance;
}

} // namespace CeosProtocol
} // namespace Server
