#ifndef INC_CONFIG_CONFIG_H_
#define INC_CONFIG_CONFIG_H_

#include "ADS1115_main.h"

// Voltage Reference (should be 3V3)
#define VREF 3.3f

// Mechanical Power Calculation Constants
#define DISTANCE_FROM_FORCE_SENSOR_TO_CENTER_OF_SHAFT_M 1.0f
#define MOMENT_OF_INERTIA_KG_M2 1.0f

// Main PID controller parameters
#define K_P 1.0f
#define K_I 1.0f
#define K_D 1.0f
#define PID_MAX_OUTPUT 100.0f
#define THROTTLE_GAIN 1.0f
#define BRAKE_GAIN 1.0f
#define HORIZONTAL_BIAS 0.0f
#define VERTICAL_BIAS 0.0f

// User Input Config (like buttons)
#define USER_INPUT_CIRCULAR_BUFFER_SIZE 100u

// Session Controller Config
#define SESSIONCONTROLLER_TASK_OSDELAY 20u

// BPM Config
#define MIN_DUTY_CYCLE_PERCENT 0.0f
#define MAX_DUTY_CYCLE_PERCENT 0.95f
#define BPM_CIRCULAR_BUFFER_SIZE 100
#define BPM_TASK_OSDELAY 20

// FORCE SENSOR Config
#define MAX_FORCE_LBF 25.0f
#define FORCESENSOR_TASK_OSDELAY 20
#define FORCESENSOR_CIRCULAR_BUFFER_SIZE 100

// ADS1115 I2C Config
#define ADS1115_SAMPLE_SPEED ADS1115_RATE_475


// Optical Encoder Config
#define OPTICAL_MAX_NUM_OVERFLOWS 3 // Meant to count overflows for optical encoder
#define NUM_APERTURES 64 // Tied to physical 3D printed apparatus
#define OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE 100 // Need to evaluate maximum possible size from STM32
#define OPTICAL_ENCODER_TASK_OSDELAY 20

// PID config
#define PID_INITIAL_STATUS false
#define PID_TASK_OSDELAY 20

// USB config
#define USB_TX_BUFFER_SIZE 512 // Buffer that is being sent to USB peripheral
#define USB_TASK_OSDELAY 20

// LCD config
#define LCD_TASK_OSDELAY 20
#define SESSION_CONTROLLER_TO_LUMEX_LCD_MSG_STRING_SIZE 16 + 1

// LED config
#define LED_TASK_OSDELAY 500

// Error and Warning settings
#define TASK_ERROR_CIRCULAR_BUFFER_SIZE 50
#define TASK_WARNING_RETRY_OSDELAY 100

// Task Monitor config
#define TASK_MONITOR_TASK_OSDELAY 1000





#endif /* INC_CONFIG_CONFIG_H_ */
