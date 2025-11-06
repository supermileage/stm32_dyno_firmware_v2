#ifndef INC_OPTICALSENSOR_ADC_H_
#define INC_OPTICALSENSOR_ADC_H_

#include "main.h"
#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void optical_sensor_interrupt(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* timer);
void optical_sensor_main(ADC_HandleTypeDef* adcHandle, osMessageQueueId_t sessionControllerToForceSensorADCHandle, osMessageQueueId_t forceSensorADCToSessionControllerHandle);

#ifdef __cplusplus
}
#endif

#endif INC_OPTICALSENSOR_ADC_H_
