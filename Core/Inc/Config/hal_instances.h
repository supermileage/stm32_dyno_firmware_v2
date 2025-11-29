#ifndef INC_CONFIG_HAL_INSTANCES_H_
#define INC_CONFIG_HAL_INSTANCES_H_



#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TIM_HandleTypeDef* lumexLcdTimer;
extern TIM_HandleTypeDef* bpmTimer;
extern TIM_HandleTypeDef* opticalTimer;

extern ADC_HandleTypeDef* forceSensorADCHandle;

extern I2C_HandleTypeDef* forceSensorADS1115Handle;

#ifdef __cplusplus
}
#endif

#endif // INC_CONFIG_HAL_INSTANCES_H_