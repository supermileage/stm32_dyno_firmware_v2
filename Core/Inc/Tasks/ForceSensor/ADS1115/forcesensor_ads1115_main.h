#ifndef INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_MAIN_H_
#define INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_MAIN_H_

#include "main.h"
#include "cmsis_os2.h"

#include "config.h"

#include "ADS1115.h"

#include "messagePassing/osqueue_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

void force_sensor_ads1115_gpio_alert_interrupt(void);
void force_sensor_ads1115_main(I2C_HandleTypeDef* i2cHandle, osMessageQueueId_t sessionControllerToForcesensorADS1115Handle);

#ifdef __cplusplus
}
#endif



#endif /* INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_MAIN_H_ */
