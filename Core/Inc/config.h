#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "ADS1115_main.h"

// Power Calculation Constants
#define DISTANCE_FROM_FORCE_SENSOR_TO_CENTER_OF_SHAFT_M 1
#define MOMENT_OF_INERTIA_KG_M2 1

// Task enable/disables
#define FORCE_SENSOR_ADS1115_TASK_ENABLE 1
#define FORCE_SENSOR_ADC_TASK_ENABLE 0
#define OPTICAL_ENCODER_TASK_ENABLE 1
#define SESSION_CONTROLLER_TASK_ENABLE 1
#define PID_CONTROLLER_TASK_ENABLE 1
#define BPM_CONTROLLER_TASK_ENABLE 1
#define LUMEX_LCD_TASK_ENABLE 1
#define USB_CONTROLLER_TASK_ENABLE 1


// Clock speed for timers, need to find better way to get this
// #define CLK_SPEED 200000000

// Voltage Reference (should be 3V3)
#define VREF 3.3

// Main PID controller K_P, K_I and K_D
#define K_P 1.0
#define K_I 1.0
#define K_D 1.0

// User Input Config (like buttons)
#define USER_INPUT_CIRCULAR_BUFFER_SIZE 100

// BPM Config
#define MIN_DUTY_CYCLE_PERCENT 0.0
#define MAX_DUTY_CYCLE_PERCENT 0.95
#define BPM_CIRCULAR_BUFFER_SIZE 50

// FORCE SENSOR Config
#define MAX_FORCE_LBF 25

// ADS1115 I2C Config
#define ADS1115_SAMPLE_SPEED ADS1115_RATE_475
#define FORCESENSOR_CIRCULAR_BUFFER_SIZE 50

// Optical Encoder Config
#define OP_OF 3 // Meant to count overflows for optical encoder
#define NUM_APERTURES 64 // Tied to physical 3D printed apparatus
#define OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE 50// Need to evaluate maximum possible size from STM32

// PID config
#define PID_INITIAL_STATUS false

// USB config
#define USB_TX_BUFFER_SIZE 512 // Buffer that is being sent to USB peripheral


// USB Controller Config
// #define USB_CDC_TX_BUFFER_SIZE 2028
#endif /* INC_CONFIG_H_ */
