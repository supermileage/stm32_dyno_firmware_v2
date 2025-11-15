#ifndef INC_OPTICALSENSOR_H_
#define INC_OPTICALSENSOR_H_

#include "main.h"
#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

void optical_sensor_interrupt();
void optical_sensor_overflow_interrupt();
void optical_sensor_main(TIM_HandleTypeDef* opticalTimer, TIM_HandleTypeDef* timestampTimer, osMessageQueueId_t sessionControllerToForceSensorADCHandle, osMessageQueueId_t forceSensorADCToSessionControllerHandle);

#ifdef __cplusplus
}
#endif

#endif // INC_OPTICALSENSOR_H_
