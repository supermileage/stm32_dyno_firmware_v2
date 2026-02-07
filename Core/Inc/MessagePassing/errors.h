#ifndef INC_MESSAGEPASSING_ERRORS_H_
#define INC_MESSAGEPASSING_ERRORS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WARNING_ENUM_OFFSET 10000

typedef enum : uint32_t 
{
	TASK_ID_NO_TASK = 0xFFFF,
    TASK_ID_TASK_MONITOR = 0,
	TASK_ID_SESSION_CONTROLLER = 100,
	TASK_ID_USB_CONTROLLER,
	TASK_ID_SD_CONTROLLER,
	TASK_ID_OPTICAL_ENCODER,
	TASK_ID_FORCE_SENSOR,
	TASK_ID_BPM_CONTROLLER,
	TASK_ID_PID_CONTROLLER,
	TASK_ID_LUMEX_LCD	
} task_ids_t;

typedef struct __attribute__((packed))
{
    uint32_t timestamp;
    task_ids_t task_id;
    uint32_t error_id;
} task_error_data;

#ifdef STM32H7xx_H
_Static_assert(sizeof(task_error_data) == 4 + 4 + 4, "Size of task_error_data must be 12 bytes");
#else
static_assert(sizeof(task_error_data) == 4 + 4 + 4, "Size of task_error_data must be 12 bytes");
#endif

inline task_error_data PopulateTaskErrorDataStruct(uint32_t timestamp, task_ids_t task_id, uint32_t error_id)
{
    task_error_data error_data;
    error_data.timestamp = timestamp;
    error_data.task_id = task_id;
    error_data.error_id = error_id;
    return error_data;
}

#ifdef STM32H7xx_H
_Static_assert(sizeof(task_ids_t) == 4, "Size of task_id must be 4 bytes");
#else
static_assert(sizeof(task_ids_t) == 4, "Size of task_id must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_SESSION_CONTROLLER_TIMESTAMP_TIMER_START_FAILURE = 0,
    ERROR_SESSION_CONTROLLER_INVALID_TASK_QUEUE_POINTER,
    ERROR_SESSION_CONTROLLER_INVALID_UART1_MUTEX_POINTER
} session_controller_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(session_controller_task_error_ids) == 4, "Size of session_controller_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(session_controller_task_error_ids) == 4, "Size of session_controller_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_BPM_PWM_START_FAILURE = 0,
    ERROR_BPM_PWM_STOP_FAILURE
} bpm_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(bpm_task_error_ids) == 4, "Size of bpm_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(bpm_task_error_ids) == 4, "Size of bpm_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_LUMEX_LCD_TIMER_START_FAILURE = 0
} lumex_lcd_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(lumex_lcd_task_error_ids) == 4, "Size of lumex_lcd_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(lumex_lcd_task_error_ids) == 4, "Size of lumex_lcd_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_TASK_MONITOR_INVALID_THREAD_ID_POINTER = 0
} task_monitor_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(task_monitor_task_error_ids) == 4, "Size of task_monitor_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(task_monitor_task_error_ids) == 4, "Size of task_monitor_task_error_ids must be 4 bytes");
#endif
    
typedef enum : uint32_t
{
    ERROR_PID_INVALID_UART1_MUTEX_POINTER = 0,
    WARNING_PID_CONTROLLER_MESSAGE_QUEUE_FULL = 10000
} pid_controller_task_error_ids;  

#ifdef STM32H7xx_H
_Static_assert(sizeof(pid_controller_task_error_ids) == 4, "Size of pid_controller_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(pid_controller_task_error_ids) == 4, "Size of pid_controller_task_error_ids must be 4 bytes");
#endif


#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_ERRORS_H_
