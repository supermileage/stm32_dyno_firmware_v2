#include "TimeKeeping/timestamps.h"

uint32_t get_timestamp()
{
    return __HAL_TIM_GET_COUNTER(timestampTimer);
}