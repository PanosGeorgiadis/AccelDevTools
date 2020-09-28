#pragma once

#include <map>
#include <mutex>
#include <string>
#include <chrono>
#include <cpplib/com/atlcom_api.h>
#include <cpplib/types/interface.h>
#include <cpplib/types/null_object.h>
#include <cpplib/link_cpplib.h>

namespace cpp
{
namespace com
{

//-----------------------------------------------------------------------------------------------------------------------------------------

class TrackedProxyLoggingItf :
    public Interface
{
public:
    virtual void log_info(const std::wstring&) const = 0; // Severity::Informational, Category::Notification
};

//-----------------------------------------------------------------------------------------------------------------------------------------

class NullTrackedProxyLogger :
    public NullObject<TrackedProxyLoggingItf, NullTrackedProxyLogger>
{
public:
    virtual void log_info(const std::wstring&) const override {}
};

//-----------------------------------------------------------------------------------------------------------------------------------------

class TrackedProxyItf :
    public Interface
{
public:
    virtual void Disconnect() = 0;
};

// Keeps track of active and disconnecting proxies, allowing us to wait for them to be destroyed before shutdown 
class ActiveProxyTracker final
{
public:
    ActiveProxyTracker() = delete;
    ~ActiveProxyTracker();
    explicit ActiveProxyTracker(const TrackedProxyLoggingItf& log);
    ActiveProxyTracker(const ActiveProxyTracker&) = delete;
    ActiveProxyTracker& operator=(const ActiveProxyTracker&) = delete;
    ActiveProxyTracker(ActiveProxyTracker&&) = delete;

    void AddProxy(TrackedProxyItf* proxy, const std::string& proxyName);
    void RemoveProxy(TrackedProxyItf* pProxy);
    
    void OnExternalClientConnectsToProxy(TrackedProxyItf* pProxy);
    void OnLastExternalClientDisconnectsFromProxy(TrackedProxyItf* pProxy);

    void DisconnectAndWaitForAllProxies();
    void DisconnectAndWaitForAllProxies(const std::chrono::steady_clock::duration& timeout);

    size_t GetConnectedProxyCount() const noexcept;
    size_t GetTotalProxyCount() const noexcept;

    void Start();
    void Stop();

private:
    using Mutex = std::recursive_mutex; 
    using LockGuard = std::lock_guard<Mutex>;
    using ProxyCollection = std::map<TrackedProxyItf*,std::shared_ptr<std::string> >;

    bool HasTrackedProxies() const;
    void DisconnectAllProxies();
    void WaitForProxiesToBeDestroyed(const std::chrono::steady_clock::duration& wait_time) const;
    void LogDisconnectingProxies() const;

    mutable Mutex m_mutex;
    ProxyCollection m_connectedProxies; // proxies connected to external clients
    ProxyCollection m_trackedProxies;
    bool m_isActive;
    const TrackedProxyLoggingItf& m_log;
};

} // com 
} // cpp
