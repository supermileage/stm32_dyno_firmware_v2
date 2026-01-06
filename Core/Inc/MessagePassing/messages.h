#ifndef INC_MESSAGEPASSING_MESSAGES_H_
#define INC_MESSAGEPASSING_MESSAGES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "cmsis_os2.h"

#include "Config/config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opcodes for controlling the Lumex LCD display from the session controller
typedef enum : uint32_t
{
	CLEAR_DISPLAY = 0,      // Clear the entire display
	WRITE_TO_DISPLAY = 1,   // Write a string to a specific location on the display
	ENABLE_BLINK_ON_CURSOR = 2,    // Enable blinking cursor
	DISABLE_BLINK_ON_CURSOR = 3    // Disable blinking cursor
} session_controller_to_lumex_lcd_opcode;

_Static_assert(sizeof(session_controller_to_lumex_lcd_opcode) == 4, "Size of session_controller_to_lumex_lcd_opcode must be 4 bytes");

// Message sent from the session controller to the Lumex LCD
typedef struct {
	session_controller_to_lumex_lcd_opcode op;  // Operation to perform on the display
    uint32_t row;                                // Row number on the LCD
    uint32_t column;                             // Column number on the LCD
	size_t size;
	char display_string[SESSION_CONTROLLER_TO_LUMEX_LCD_MSG_STRING_SIZE];                       // String to write (if WRITE_TO_DISPLAY)
} session_controller_to_lumex_lcd;

_Static_assert(sizeof(session_controller_to_lumex_lcd) >= 
               offsetof(session_controller_to_lumex_lcd, display_string) + sizeof(((session_controller_to_lumex_lcd *)0)->display_string),
               "Size of session_controller_to_lumex_lcd must be correct");


// Opcodes for controlling the BPM (Pulse Width Modulation) module from the session controller
typedef enum : uint32_t
{
	READ_FROM_PID = 0,		// read from PID Controller
	START_PWM,         		// Start PWM output
	STOP_PWM              // Stop PWM output
} session_controller_to_bpm_opcode;

_Static_assert(sizeof(session_controller_to_bpm_opcode) == 4, "Size of session_controller_to_bpm_opcode must be 4 bytes");

// Message sent from the session controller to the BPM module
typedef struct {
	session_controller_to_bpm_opcode op;  // Operation to perform on the BPM
	float new_duty_cycle_percent;              		// New duty cycle percentage from 0 - 1
} session_controller_to_bpm;

_Static_assert(sizeof(session_controller_to_bpm) == 4 + 4, "Size of session_controller_to_bpm must be 8 bytes");


// Message sent from the session controller to the PID controller
typedef struct
{
	bool enable_status;   // Enable or disable the PID controller
	float desired_angular_velocity;    // Desired motor RPM setpoint
} session_controller_to_pid_controller;

_Static_assert(sizeof(session_controller_to_pid_controller) == 4 + 4, "Size of session_controller_to_pid_controller must be 8 bytes");

// Message sent from the optical encoder to the PID controller
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

typedef enum : uint32_t 
{
	TASK_MONITOR_SESSION_CONTROLLER_TASK = 0,
	TASK_MONITOR_USB_CONTROLLER_TASK,
	TASK_MONITOR_SD_CONTROLLER_TASK,
	TASK_MONITOR_OPTICAL_ENCODER_TASK,
	TASK_MONITOR_FORCE_SENSOR_TASK,
	TASK_MONITOR_BPM_CONTROLLER_TASK,
	TASK_MONITOR_PID_CONTROLLER_TASK,
	TASK_MONITOR_LUMEX_LCD_TASK,
	TASK_MONITOR_TASK_MONITOR_TASK
} task_monitor_task_opcode;

_Static_assert(sizeof(task_monitor_task_opcode) == 4, "Size of task_monitor_task_opcode must be 4 bytes");

typedef struct
{
	uint32_t timestamp;
	task_monitor_task_opcode op;
	osThreadState_t task_state;
	uint32_t free_bytes;
} task_monitor_to_usb_controller;

_Static_assert(sizeof(task_monitor_to_usb_controller) == 4 + 4 + 4 + 4, "Size of task_monitor_to_usb_controller must be 16 bytes");


#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_MESSAGES_H_
