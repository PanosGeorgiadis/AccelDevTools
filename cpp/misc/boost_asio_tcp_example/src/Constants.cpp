#include "CeosProtocol/Constants.h"

namespace Server {
namespace CeosProtocol {

uint32_t SwapBytes(uint32_t value)
{
    uint32_t retval = value & 0xFFU;
    retval = (retval << 8U) | ((value >> 8U) & 0xFFU);
    retval = (retval << 8U) | ((value >> 16U) & 0xFFU);
    retval = (retval << 8U) | ((value >> 24U) & 0xFFU);
    return retval;
}

} // namespace CeosProtocol
} // namespace Server
