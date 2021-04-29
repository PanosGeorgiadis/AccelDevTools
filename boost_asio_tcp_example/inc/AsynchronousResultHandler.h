#pragma once

#include <boost/asio.hpp>
#include "Generic/noncopyable.h"
#include "CeosProtocol/RawData.h"

namespace Server {
namespace CeosProtocol {

class AsynchronousResultHandler :
    public Infra::Generic::NonCopyable
{
private:
public:
    explicit AsynchronousResultHandler(const std::shared_ptr<RawData>& pRawData, const std::function<void(const std::shared_ptr<RawData>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler);
    void HandleData() const;
    void HandleError(const boost::system::error_code& error) const;
    RawData& Data() const;

private:
    std::function<void(const std::shared_ptr<RawData>&)> m_handler;
    std::function<void(const boost::system::error_code& error)> m_errorHandler;
    std::shared_ptr<RawData> m_rawData;
};

} // namespace Ceos
} // namespace Server
