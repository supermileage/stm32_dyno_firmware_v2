#ifndef INC_MAIN_BOARD_MESSAGEPASSING_MESSAGES_PUBLIC_H_
#define INC_MAIN_BOARD_MESSAGEPASSING_MESSAGES_PUBLIC_H_

#include <stdint.h>
#include <assert.h>

// A task error/warning is reported as a single 32-bit code:
//   bits 31..16 : task offset (unique per task, see task_offset_t)
//   bit  15     : warning flag (set => warning, clear => error)
//   bits 14..0  : task-local error number
// An error code is formed by OR-ing the task offset with the error number, so the
// task id no longer needs to be sent as a separate field.
#define TASK_OFFSET_SHIFT    16u
#define WARNING_FLAG         (1u << 15)
#define TASK_ERROR_NUM_MASK  (WARNING_FLAG - 1u)              // bits 0..14
#define TASK_OFFSET_MASK     (0xFFFFu << TASK_OFFSET_SHIFT)   // bits 16..31

#ifdef __cplusplus
extern "C" {
#endif

// ****************************************************
// ERRORS AND WARNINGS
// ****************************************************
// Unique per-task offset occupying the high bits of an error code. OR-ed with a
// task-local error number to form the error_code sent over USB.
typedef enum : uint32_t
{
	TASK_OFFSET_NO_TASK              = 0xFFFFu << TASK_OFFSET_SHIFT,
	TASK_OFFSET_TASK_MONITOR         = 0u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_SESSION_CONTROLLER   = 1u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_USB_CONTROLLER       = 2u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_SD_CONTROLLER        = 3u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_OPTICAL_ENCODER      = 4u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_FORCE_SENSOR_ADC     = 5u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_FORCE_SENSOR_ADS1115 = 6u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_BPM_CONTROLLER       = 7u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_PID_CONTROLLER       = 8u  << TASK_OFFSET_SHIFT,
	TASK_OFFSET_LUMEX_LCD            = 9u  << TASK_OFFSET_SHIFT
} task_offset_t;

typedef struct __attribute__((packed))
{
    uint32_t timestamp;
    uint32_t error_code;   // task_offset | (warning ? WARNING_FLAG : 0) | error number
} task_error_data;

#ifdef STM32H7xx_H
_Static_assert(sizeof(task_error_data) == 4 + 4, "Size of task_error_data must be 8 bytes");
#else
static_assert(sizeof(task_error_data) == 4 + 4, "Size of task_error_data must be 8 bytes");
#endif

static inline task_error_data PopulateTaskErrorDataStruct(uint32_t timestamp, task_offset_t task_offset, uint32_t error_id)
{
    task_error_data error_data;
    error_data.timestamp = timestamp;
    error_data.error_code = (uint32_t)task_offset | error_id;
    return error_data;
}

#ifdef STM32H7xx_H
_Static_assert(sizeof(task_offset_t) == 4, "Size of task_offset_t must be 4 bytes");
#else
static_assert(sizeof(task_offset_t) == 4, "Size of task_offset_t must be 4 bytes");
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
    WARNING_PID_CONTROLLER_MESSAGE_QUEUE_FULL = WARNING_FLAG
} pid_controller_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(pid_controller_task_error_ids) == 4, "Size of pid_controller_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(pid_controller_task_error_ids) == 4, "Size of pid_controller_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_FORCE_SENSOR_ADC_START_FAILURE = 0
} force_sensor_adc_task_error_ids;

#ifdef STM32H7xx_H
_Static_assert(sizeof(force_sensor_adc_task_error_ids) == 4, "Size of force_sensor_adc_task_error_ids must be 4 bytes");
#else
static_assert(sizeof(force_sensor_adc_task_error_ids) == 4, "Size of force_sensor_adc_task_error_ids must be 4 bytes");
#endif

typedef enum : uint32_t
{
    ERROR_FORCE_SENSOR_ADS1115_INIT_FAILURE = 0,
    WARNING_FORCE_SENSOR_ADS1115_TRIGGER_CONVERSION_FAILURE = WARNING_FLAG,
    WARNING_FORCE_SENSOR_ADS1115_GET_CONVERSION_FAILURE
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
    task_offset_t  task_offset;  // which module owns payload
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
	task_offset_t task_offset;
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