#ifndef INC_MESSAGEPASSING_MESSAGES_H_
#define INC_MESSAGEPASSING_MESSAGES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Config/config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opcodes for controlling the Lumex LCD display from the session controller
typedef enum
{
	CLEAR_DISPLAY = 0,      // Clear the entire display
	WRITE_TO_DISPLAY = 1,   // Write a string to a specific location on the display
} session_controller_to_lumex_lcd_opcode;

// Message sent from the session controller to the Lumex LCD
typedef struct {
	session_controller_to_lumex_lcd_opcode op;  // Operation to perform on the display
    uint8_t row;                                // Row number on the LCD
    uint8_t column;                             // Column number on the LCD
    char display_string[SESSION_CONTROLLER_TO_LUMEX_LCD_MSG_STRING_SIZE];                       // String to write (if WRITE_TO_DISPLAY)
	size_t size;
} session_controller_to_lumex_lcd;

// Opcodes for controlling the BPM (Pulse Width Modulation) module from the session controller
typedef enum
{
	READ_FROM_PID = 0,		// read from PID Controller
	START_PWM,         		// Start PWM output
	STOP_PWM              // Stop PWM output
} session_controller_to_bpm_opcode;

// Message sent from the session controller to the BPM module
typedef struct {
	session_controller_to_bpm_opcode op;  // Operation to perform on the BPM
	float new_duty_cycle_percent;              		// New duty cycle percentage from 0 - 1
} session_controller_to_bpm;


// Message sent from the session controller to the PID controller
typedef struct
{
	bool enable_status;   // Enable or disable the PID controller
	float desired_angular_velocity;    // Desired motor RPM setpoint
} session_controller_to_pid_controller;

typedef enum {
	OPTICAL_ENCODER = 0,
	FORCESENSOR, 
	BRAKE_PWM
} send_usb_output_data;

// Message sent from the optical encoder to the PID controller
typedef struct
{
	uint32_t timestamp;  // Timestamp of the reading 
	float angular_velocity; // Measured angular velocity
	uint32_t raw_value;  // In case users want to have custom implementation with it
	float angular_acceleration; // Measured angular acceleration
} optical_encoder_output_data;


typedef struct {
	uint32_t timestamp;
	float force;
	uint32_t raw_value;
} forcesensor_output_data;

typedef struct
{
    uint32_t timestamp;
    float duty_cycle;
	uint32_t raw_value; // Really just padding to match the other output data types
} bpm_output_data;

#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_MESSAGES_H_
