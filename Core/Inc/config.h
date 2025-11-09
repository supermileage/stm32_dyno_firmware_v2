#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "ADS1115.h"

// Voltage Reference (should be 3V3)
#define VREF 3.3

// BPM MIN and MAX duty cycle percentages from 0 - 1
#define MIN_DUTY_CYCLE_PERCENT 0.0
#define MAX_DUTY_CYCLE_PERCENT 0.95

// Main PID controller K_P, K_I and K_D
#define K_P 1.0
#define K_I 1.0
#define K_D 1.0

// ADS1115 I2C ADC SAMPLE_SPEED
#define ADS1115_SAMPLE_SPEED ADS1115_RATE_475

// Task enable/disables
#define FORCE_SENSOR_ADS1115_TASK_ENABLE 1
#define FORCE_SENSOR_ADC_TASK_ENABLE 0


#endif /* INC_CONFIG_H_ */
