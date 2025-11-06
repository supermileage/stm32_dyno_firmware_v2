#ifndef INC_OSQUEUE_TASK_TO_TASK_H_
#define INC_OSQUEUE_TASK_TO_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cmsis_os.h"

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
    char* display_string;                       // String to write (if WRITE_TO_DISPLAY)
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
	float desired_rpm;    // Desired motor RPM setpoint
} session_controller_to_pid_controller;

// Message sent from the optical encoder to the PID controller
typedef struct
{
	uint32_t timestamp;  // Timestamp of the reading
	float rpm;           // Measured RPM from the encoder
} optical_encoder_output_data;


typedef struct {
	uint32_t timestamp;
	float force;
} forcesensor_output_data;

typedef struct {
	uint32_t timestamp_os;
	float placeholder;
} optical_sensor_output_data;

#ifdef __cplusplus
extern "C" {
#endif

void EmptyQueue(osMessageQueueId_t qHandle, size_t itemSize);
bool GetLatestFromQueue(osMessageQueueId_t queueHandle, void* latestData, size_t itemSize, uint32_t timeout);

#ifdef __cplusplus
}
#endif



#endif /* INC_OSQUEUE_TASK_TO_TASK_H_ */
