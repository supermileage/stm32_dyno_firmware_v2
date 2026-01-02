#ifndef INC_MESSAGEPASSING_ERRORS_H_
#define INC_MESSAGEPASSING_ERRORS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 0 - 9999: Errors
// 10000 and above: Warnings

// 0 - 99 : Session Controller Task Errors
// 100 - 199 : Usb Controller Task Errors
// 200 - 299 : SD Card Task Errors
// 300 - 399 : Optical Encoder Task Errors
// 400 - 499 : Force Sensor Task Errors
// 500 - 599 : BPM Controller Task Errors
// 600 - 699 : PID Controller Task Errors
// 700 - 799 : Lumex LCD Task Errors
// 800 - 899 : Task Monitor Task Errors

// Warnings start at 10000 with same sub-ranges as errors


typedef enum : uint32_t
{
    ERROR_SESSION_CONTROLLER_TIMESTAMP_TIMER_START_FAILURE = 0,
    ERROR_SESSION_CONTROLLER_INVALID_TASK_QUEUE_POINTER,
    ERROR_FORCE_SENSOR_ADC_START_FAILURE = 400,
    ERROR_FORCE_SENSOR_ADS1115_INIT_FAILURE,
    ERROR_BPM_PWM_START_FAILURE = 500,
    ERROR_BPM_PWM_STOP_FAILURE,
    ERROR_LUMEX_LCD_TIMER_START_FAILURE = 700,
    ERROR_TASK_MONITOR_INVALID_THREAD_ID_POINTER = 800,
    FIRST_WARNING_NUMBER = 10000,
    WARNING_FORCE_SENSOR_ADS1115_TRIGGER_CONVERSION_FAILURE = 10400,
    WARNING_FORCE_SENSOR_ADS1115_GET_CONVERSION_FAILURE,
    WARNING_PID_CONTROLLER_MESSAGE_QUEUE_FULL = 10600,

} task_errors;



#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_ERRORS_H_