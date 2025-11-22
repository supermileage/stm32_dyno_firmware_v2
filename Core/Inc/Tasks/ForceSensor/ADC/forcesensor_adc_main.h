#ifndef INC_FORCESENSOR_ADC_FORCESENSOR_ADC_MAIN_H_
#define INC_FORCESENSOR_ADC_FORCESENSOR_ADC_MAIN_H_

#include "main.h"
#include "cmsis_os2.h"

#include "MessagePassing/osqueue_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

void adc_forcesensor_interrupt(ADC_HandleTypeDef* hadc);
void force_sensor_adc_main(ADC_HandleTypeDef* adcHandle, osMessageQueueId_t sessionControllerToForceSensorADCHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_FORCESENSOR_ADC_FORCESENSOR_ADC_MAIN_H_ */
