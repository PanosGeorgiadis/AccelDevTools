#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "Generic/noncopyable.h"

namespace Server {
namespace CeosProtocol {

template <typename T>
class AsynchronousResult :
    public Infra::Generic::NonCopyable
{
public:
    void HandleData(const std::shared_ptr<T>& pData) 
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_pData = pData;
        m_cond.notify_one();
    }

    void HandleError(const boost::system::error_code& error)
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_error = error;
        m_cond.notify_one();
    }

    std::shared_ptr<T> Get()
    {
        boost::mutex::scoped_lock lock(m_mutex);
        m_cond.wait(lock, [this] () { return m_pData != nullptr || m_error; });
        
        if (m_error)
            throw boost::system::system_error(m_error);
        
        return m_pData;
    }
    
private:
    boost::mutex m_mutex;
    boost::condition m_cond;

    std::shared_ptr<T> m_pData;
    boost::system::error_code m_error;
};

} // namespace Ceos
} // namespace Server
