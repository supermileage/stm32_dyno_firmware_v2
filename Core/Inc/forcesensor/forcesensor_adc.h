#ifndef INC_FORCESENSOR_ADC_H_
#define INC_FORCESENSOR_ADC_H_

#include "main.h"
#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void adc_forcesensor_interrupt(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* timer);
void force_sensor_adc_main(osMessageQueueId_t sessionControllerToForceSensorADCHandle, osMessageQueueId_t forceSensorADCToSessionControllerHandle, ADC_HandleTypeDef* adcHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_FORCESENSOR_ADC_H_ */
