#ifndef INC_MESSAGEPASSING_MSGQ_MESSAGES_H_
#define INC_MESSAGEPASSING_MSGQ_MESSAGES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "cmsis_os2.h"

#include "Config/config.h"
#include "usb_messages.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opcodes for controlling the Lumex LCD display from the session controller
typedef enum : uint32_t
{
	CLEAR_DISPLAY = 0,      // Clear the entire display
	WRITE_TO_DISPLAY = 1   // Write a string to a specific location on the display
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


#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_MSGQ_MESSAGES_H_
