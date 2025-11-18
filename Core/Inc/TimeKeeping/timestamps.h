#ifndef INC_TIMEKEEPING_TIMESTAMPS_H_
#define INC_TIMEKEEPING_TIMESTAMPS_H_

#include <stdint.h>

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TIM_HandleTypeDef* timestampTimer;

uint32_t get_timestamp();

#ifdef __cplusplus
}
#endif

#endif // INC_TIMEKEEPING_TIMESTAMPS_H_