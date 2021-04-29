#pragma once

#include <stdint.h>
#include "CeosProtocol/Constants.h"

namespace Server {
namespace CeosProtocol {
namespace Supply {

const uint32_t MINORNUMBER = 0U;
const uint32_t BACKWARDNUMBER = 0U;
const uint32_t CLIENTTYPE = 8U;

const uint32_t COMMAND_TYPE_OFFSET = 2011600U;
const uint32_t COMMAND_SET_VALUE = COMMAND_TYPE_OFFSET + 1U;
const uint32_t COMMAND_INCREMENT = COMMAND_TYPE_OFFSET + 2U;
const uint32_t COMMAND_DECREMENT = COMMAND_TYPE_OFFSET + 3U;
const uint32_t COMMAND_GET_VALUE = COMMAND_TYPE_OFFSET + 4U;
const uint32_t COMMAND_GET_MAX = COMMAND_TYPE_OFFSET + 5U;
const uint32_t COMMAND_GET_MIN = COMMAND_TYPE_OFFSET + 6U;
const uint32_t COMMAND_GET_RESOLUTION = COMMAND_TYPE_OFFSET + 7U;

const uint32_t RESULT_ERROR_NO_SUCH_ELEMENT = CeosProtocol::RESULTCODES + 20U;

} // namespace Supply
} // namespace CeosProtocol
} // namespace Server
