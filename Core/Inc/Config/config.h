#ifndef INC_CONFIG_CONFIG_H_
#define INC_CONFIG_CONFIG_H_

#include "ADS1115_main.h"

// Peripheral enable/disables
// ===== GPIO =====
#define STM32_PERIPHERAL_GPIO_ENABLE      1

// ===== TIMERS =====
#define STM32_PERIPHERAL_TIM1_ENABLE      1
#define STM32_PERIPHERAL_TIM2_ENABLE      1
#define STM32_PERIPHERAL_TIM13_ENABLE     1
#define STM32_PERIPHERAL_TIM14_ENABLE     1
#define STM32_PERIPHERAL_TIM16_ENABLE     1

// ===== ADC =====
#define STM32_PERIPHERAL_ADC2_ENABLE      1
#define STM32_PERIPHERAL_ADC3_ENABLE      1

// ===== SPI =====
#define STM32_PERIPHERAL_SPI1_ENABLE      1
#define STM32_PERIPHERAL_SPI2_ENABLE      1

// ===== UART =====
#define STM32_PERIPHERAL_USART1_ENABLE    1

// ===== SDMMC / STORAGE =====
#define STM32_PERIPHERAL_SDMMC1_ENABLE    0

// ===== I2C =====
#define STM32_PERIPHERAL_I2C4_ENABLE      1


// Task enable/disables
#define FORCE_SENSOR_ADS1115_TASK_ENABLE 0
#define FORCE_SENSOR_ADC_TASK_ENABLE 0
#define OPTICAL_ENCODER_TASK_ENABLE 0 
#define SESSION_CONTROLLER_TASK_ENABLE 1
#define PID_CONTROLLER_TASK_ENABLE 0
#define BPM_CONTROLLER_TASK_ENABLE 0
#define LUMEX_LCD_TASK_ENABLE 1
#define USB_CONTROLLER_TASK_ENABLE 0
#define LED_BLINK_TASK_ENABLE 1

// Voltage Reference (should be 3V3)
#define VREF 3.3

// Mechanical Power Calculation Constants
#define DISTANCE_FROM_FORCE_SENSOR_TO_CENTER_OF_SHAFT_M 1
#define MOMENT_OF_INERTIA_KG_M2 1

// Main PID controller K_P, K_I and K_D
#define K_P 1.0
#define K_I 1.0
#define K_D 1.0

// User Input Config (like buttons)
#define USER_INPUT_CIRCULAR_BUFFER_SIZE 100

// Session Controller Config
#define SESSIONCONTROLLER_TASK_OSDELAY 20

// BPM Config
#define MIN_DUTY_CYCLE_PERCENT 0.0
#define MAX_DUTY_CYCLE_PERCENT 0.95
#define BPM_CIRCULAR_BUFFER_SIZE 50
#define BPM_TASK_OSDELAY 20

// FORCE SENSOR Config
#define MAX_FORCE_LBF 25
#define FORCESENSOR_TASK_OSDELAY 20
#define FORCESENSOR_CIRCULAR_BUFFER_SIZE 50

// ADS1115 I2C Config
#define ADS1115_SAMPLE_SPEED ADS1115_RATE_475


// Optical Encoder Config
#define OP_OF 3 // Meant to count overflows for optical encoder
#define NUM_APERTURES 64 // Tied to physical 3D printed apparatus
#define OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE 50// Need to evaluate maximum possible size from STM32
#define OPTICAL_ENCODER_TASK_OSDELAY 20

// PID config
#define PID_INITIAL_STATUS false
#define PID_TASK_OSDELAY 20

// USB config
#define USB_TX_BUFFER_SIZE 512 // Buffer that is being sent to USB peripheral
#define USB_TASK_OSDELAY 20 

// LCD config
#define LCD_TASK_OSDELAY 20

// LED config
#define LED_TASK_OSDELAY 500




#endif /* INC_CONFIG_CONFIG_H_ */
