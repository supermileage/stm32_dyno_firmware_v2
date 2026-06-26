#ifndef INC_MAIN_BOARD_MESSAGEPASSING_MESSAGES_PUBLIC_H_
#define INC_MAIN_BOARD_MESSAGEPASSING_MESSAGES_PUBLIC_H_

#include <stdint.h>
#include <assert.h>

#define WARNING_ENUM_OFFSET 10000

#ifdef __cplusplus
extern "C" {
#endif

// ****************************************************
// ERRORS AND WARNINGS
// ****************************************************
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

static inline task_error_data PopulateTaskErrorDataStruct(uint32_t timestamp, task_ids_t task_id, uint32_t error_id)
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
    WARNING_PID_CONTROLLER_MESSAGE_QUEUE_FULL = 10000
} pid_controller_task_error_ids;  

#ifdef STM32H7xx_H
_Static_assert(sizeof(pid_controller_task_error_ids) == 4, "Size of pid_controller_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(pid_controller_task_error_ids) == 4, "Size of pid_controller_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_FORCE_SENSOR_ADC_START_FAILURE = 0,
    WARNING_FORCE_SENSOR_ADS1115_TRIGGER_CONVERSION_FAILURE = 10000,
    WARNING_FORCE_SENSOR_ADS1115_GET_CONVERSION_FAILURE
} force_sensor_adc_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(force_sensor_adc_task_error_ids) == 4, "Size of force_sensor_adc_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(force_sensor_adc_task_error_ids) == 4, "Size of force_sensor_adc_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_FORCE_SENSOR_ADS1115_INIT_FAILURE = 0
} force_sensor_ads1115_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(force_sensor_ads1115_error_ids) == 4, "Size of force_sensor_ads1115_error_ids must be 4 bytes");
#else
static_assert(sizeof(force_sensor_ads1115_error_ids) == 4, "Size of force_sensor_ads1115_error_ids must be 4 bytes");
#endif


// ****************************************************
// USB AND PUBLIC MESSAGES
// ****************************************************
typedef enum : uint32_t {
    USB_MSG_INVALID = 0,

    USB_MSG_COMMAND,        // PC → STM32 (do something)
    USB_MSG_RESPONSE,       // STM32 → PC (reply to command)
    USB_MSG_EVENT,          // STM32 → PC (async event)
    USB_MSG_STREAM,         // STM32 → PC (continuous data)
    USB_MSG_CONFIG,         // PC → STM32 (set parameters)
    USB_MSG_STATUS,         // STM32 → PC (health / state)

    USB_MSG_ERROR,          // STM32 → PC (error report)
    USB_MSG_WARNING,        // STM32 → PC (warning report)
} usb_msg_type_t;

#ifdef STM32H7xx_H
_Static_assert(sizeof(usb_msg_type_t) == 4, "Size of usb_msg_type_t must be 4 bytes");
#else
static_assert(sizeof(usb_msg_type_t) == 4, "Size of usb_msg_type_t must be 4 bytes");
#endif

typedef struct __attribute__((packed)) {
    usb_msg_type_t msg_type;     // protocol-level intent
    task_ids_t  task_id;    // which module owns payload
    uint32_t payload_len;  // bytes following header
} usb_msg_header_t;

#ifdef STM32H7xx_H
_Static_assert(sizeof(usb_msg_header_t) == 12, "Size of usb_msg_header_t must be 12 bytes");
#else
static_assert(sizeof(usb_msg_header_t) == 12, "Size of usb_msg_header_t must be 12 bytes");
#endif

typedef struct
{
	uint32_t timestamp;  // Timestamp of the reading 
	float angular_velocity; // Measured angular velocity
	uint32_t raw_value;  // In case users want to have custom implementation with it
	float angular_acceleration; // Measured angular acceleration
} optical_encoder_output_data;

#ifdef STM32H7xx_H
_Static_assert(sizeof(optical_encoder_output_data) == 4 + 4 + 4 + 4, "Size of optical_encoder_output_data must be 16 bytes");
#else
static_assert(sizeof(optical_encoder_output_data) == 4 + 4 + 4 + 4, "Size of optical_encoder_output_data must be 16 bytes");
#endif

typedef struct {
	uint32_t timestamp;
	float force;
	uint32_t raw_value;
} forcesensor_output_data;

#ifdef STM32H7xx_H
_Static_assert(sizeof(forcesensor_output_data) == 4 + 4 + 4, "Size of forcesensor_output_data must be 12 bytes");
#else
static_assert(sizeof(forcesensor_output_data) == 4 + 4 + 4, "Size of forcesensor_output_data must be 12 bytes");
#endif

typedef struct
{
    uint32_t timestamp;
    float duty_cycle;
	uint32_t raw_value; // Really just padding to match the other output data types
} bpm_output_data;

#ifdef STM32H7xx_H
_Static_assert(sizeof(bpm_output_data) == 4 + 4 + 4, "Size of bpm_output_data must be 12 bytes");
#else
static_assert(sizeof(bpm_output_data) == 4 + 4 + 4, "Size of bpm_output_data must be 12 bytes");
#endif

typedef struct
{
	uint32_t timestamp;
	task_ids_t task_id;
	int task_state;
	uint32_t free_bytes;
} task_monitor_output_data;

#ifdef STM32H7xx_H
_Static_assert(sizeof(task_monitor_output_data) == 4 + 4 + 4 + 4, "Size of task_monitor_output_data must be 16 bytes");
#else
static_assert(sizeof(task_monitor_output_data) == 4 + 4 + 4 + 4, "Size of task_monitor_output_data must be 16 bytes");
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_MAIN_BOARD_MESSAGEPASSING_MESSAGES_PUBLIC_H_ */