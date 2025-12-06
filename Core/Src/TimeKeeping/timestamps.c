#include "TimeKeeping/timestamps.h"

uint32_t get_timestamp()
{
    return __HAL_TIM_GET_COUNTER(timestampTimer);
}

uint32_t get_clock_speed()
{
    return HAL_RCC_GetSysClockFreq();
}