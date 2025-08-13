#include "CeosProtocol/AsynchronousResultHandler.h"

namespace Server {
namespace CeosProtocol {

AsynchronousResultHandler::AsynchronousResultHandler(const std::shared_ptr<RawData>& pRawData, const std::function<void(const std::shared_ptr<RawData>&)>& handler, const std::function<void(const boost::system::error_code& error)>& errorHandler) :
    m_handler(handler),
    m_errorHandler(errorHandler),
    m_rawData(pRawData)
{
}

void AsynchronousResultHandler::HandleData() const
{
    m_handler(m_rawData);
}

void AsynchronousResultHandler::HandleError(const boost::system::error_code& error) const
{
    m_errorHandler(error);
}

RawData& AsynchronousResultHandler::Data() const
{
    return *m_rawData;
}

} // namespace CeosProtocol
} // namespace Server
