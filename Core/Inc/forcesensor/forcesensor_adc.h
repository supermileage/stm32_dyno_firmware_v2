#ifndef INC_FORCESENSOR_ADC_H_
#define INC_FORCESENSOR_ADC_H_

#include "main.h"
#include "cmsis_os2.h"
#include "xqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

void force_sensor_adc_main(TIM_HandleTypeDef* timer, osMessageQueueId_t osHandle, ADC_HandleTypeDef* adcHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_FORCESENSOR_ADC_H_ */
