#ifndef INC_SESSION_CONTROLLER_USER_INPUTS_H_
#define INC_SESSION_CONTROLLER_USER_INPUTS_H_

#include "main.h"
#include "FreeRTOS.h"
#include "config.h"

#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

// states of each of the buttons, allows to know how many times each buttons has been pressed
typedef struct {
    int32_t  encoder_ticks;         // Signed → can count direction
    int32_t encoder_switch_presses; // Count of button presses
    int32_t back_button_presses;    // Count of button presses
    int32_t select_button_presses;  // Count of button presses
    bool     brake_button_active;   // Current pressed state
} user_input_states;

typedef enum
{
    ROT_EN_TICKS,
    ROT_EN_SW,
    BTN_BACK, 
    BTN_SELECT, 
    BTN_BRAKE
} input_opcode;

// data for each "message" in the circular buffer. opcode tells which message, count will tell how many times the button has been pressed since the last time the SC checked it
// all will be interpreted as int32_t even if the data type may not always be int32
typedef struct 
{
    input_opcode opcode;
    int32_t count;
} individual_input_data;

extern volatile uint32_t input_data_index;
extern volatile individual_input_data input_data[INPUT_CIRCULAR_BUFFER_SIZE];


void registerRotaryEncoderInput();
void registerRotaryEncoderSwInput();
void registerButtonBackInput();
void registerButtonSelectInput();
void registerButtonBrakeInput();
void addToCircularBuffer(input_opcode opcode, int32_t count);


#ifdef __cplusplus
}
#endif

#endif /* INC_SESSION_CONTROLLER_USER_INPUTS_H_ */
