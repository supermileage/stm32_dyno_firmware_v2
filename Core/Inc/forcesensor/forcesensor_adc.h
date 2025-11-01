#ifndef INC_FORCESENSOR_ADC_H_
#define INC_FORCESENSOR_ADC_H_

#include "main.h"
#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"
#include "osQueue/osqueue_interrupt_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void force_sensor_adc_main(osMessageQueueId_t sc_to_fsHandle, osMessageQueueId_t fs_to_scHandle, osMessageQueueId_t adcCallbackHandle, ADC_HandleTypeDef* adcHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_FORCESENSOR_ADC_H_ */
