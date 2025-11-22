#ifndef INC_OPTICALSENSOR_H_
#define INC_OPTICALSENSOR_H_

#include "main.h"
#include "cmsis_os2.h"

#include "MessagePassing/osqueue_helpers.h"
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

void optical_sensor_output_interrupt();
void optical_sensor_overflow_interrupt();
void optical_sensor_main(TIM_HandleTypeDef* opticalTimer, osMessageQueueId_t sessionControllerToForceSensorADCHandle);

#ifdef __cplusplus
}
#endif

#endif // INC_OPTICALSENSOR_H_
