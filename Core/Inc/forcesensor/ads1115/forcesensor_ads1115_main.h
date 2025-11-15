#ifndef INC_FORCESENSOR_ADS1115_H_
#define INC_FORCESENSOR_ADS1115_H_

#include "main.h"
#include "cmsis_os2.h"

#include "config.h"

#include "ADS1115.h"

#include "osQueue/osqueue_task_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void force_sensor_ads1115_gpio_alert_interrupt(void);
void force_sensor_ads1115_main(I2C_HandleTypeDef* i2cHandle, TIM_HandleTypeDef* timestampTimer, osMessageQueueId_t sessionControllerToForcesensorADS1115Handle, osMessageQueueId_t ForcesensorADS1115ToSessionControllerHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_FORCESENSOR_ADS1115_H_ */
