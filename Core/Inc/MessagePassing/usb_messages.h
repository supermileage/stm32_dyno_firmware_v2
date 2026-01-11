#ifndef INC_MESSAGEPASSING_USB_MESSAGES_H_
#define INC_MESSAGEPASSING_USB_MESSAGES_H_

#include <stdint.h>

#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif

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

_Static_assert(sizeof(usb_msg_type_t) == 4, "Size of usb_msg_type_t must be 4 bytes");


typedef struct __attribute__((packed)) {
    usb_msg_type_t msg_type;     // protocol-level intent
    task_ids_t  module_id;    // which module owns payload
    uint32_t payload_len;  // bytes following header
} usb_msg_header_t;

_Static_assert(sizeof(usb_msg_header_t) == 12, "Size of usb_msg_header_t must be 12 bytes");

typedef struct
{
	uint32_t timestamp;  // Timestamp of the reading 
	float angular_velocity; // Measured angular velocity
	uint32_t raw_value;  // In case users want to have custom implementation with it
	float angular_acceleration; // Measured angular acceleration
} optical_encoder_output_data;

_Static_assert(sizeof(optical_encoder_output_data) == 4 + 4 + 4 + 4, "Size of optical_encoder_output_data must be 16 bytes");

typedef struct {
	uint32_t timestamp;
	float force;
	uint32_t raw_value;
} forcesensor_output_data;

_Static_assert(sizeof(forcesensor_output_data) == 4 + 4 + 4, "Size of forcesensor_output_data must be 12 bytes");

typedef struct
{
    uint32_t timestamp;
    float duty_cycle;
	uint32_t raw_value; // Really just padding to match the other output data types
} bpm_output_data;

_Static_assert(sizeof(bpm_output_data) == 4 + 4 + 4, "Size of bpm_output_data must be 12 bytes");

typedef struct
{
	uint32_t timestamp;
	task_ids_t task_id;
	int task_state;
	uint32_t free_bytes;
} task_monitor_output_data;

_Static_assert(sizeof(task_monitor_output_data) == 4 + 4 + 4 + 4, "Size of task_monitor_output_data must be 16 bytes");

#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_USB_MESSAGES_H_