#pragma once

#include <string>
#include <sstream>
#include "Generic/noncopyable.h"

namespace Server {
namespace CeosProtocol {

class Logger;

class LogEntry : public Infra::Generic::NonCopyable
{
public:
    LogEntry(const Logger& logger, long severity, long category);
    LogEntry(const Logger& logger, const std::wstring& originator, long severity, long category);
    LogEntry(LogEntry&& other):
        m_logger(std::move(other.m_logger)),
        m_hasOriginator(std::move(other.m_hasOriginator)),
        m_severity(std::move(other.m_severity)),
        m_category(std::move(other.m_category)),
        m_ss(std::move(other.m_ss))
    {}

    ~LogEntry();

    template <typename T>
    LogEntry& operator<<(const T& value)
    {
        m_ss << value;
        return *this;
    }

private:
    const Logger& m_logger;
    bool m_hasOriginator;
    std::wstring m_originator;
    long m_severity;
    long m_category;
    std::wostringstream m_ss;
};

class Logger
{
public:
    virtual ~Logger() {};
    Logger() {};
    Logger(Logger&&) {};
    virtual void Log(long severity, long category, const std::wstring& msg) const = 0;
    virtual void Log(const std::wstring& originator, long severity, long category, const std::wstring& msg) const = 0;
    virtual LogEntry Log(long severity, long category) const = 0;
    virtual LogEntry Log(const std::wstring& originator, long severity, long category) const = 0;
};

const Logger& NullLogger();

class NullLogger_impl : public Logger
{
    void Log(long, long, const std::wstring&) const override {};
    void Log(const std::wstring&, long, long, const std::wstring&) const override {};
    LogEntry Log(long severity, long category) const override { return std::move(LogEntry(*this, severity, category)); };
    LogEntry Log(const std::wstring& originator, long severity, long category) const override { return std::move(LogEntry(*this, originator, severity, category)); };
};

} // namespace Ceos
} // namespace Server
