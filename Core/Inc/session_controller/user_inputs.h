#ifndef INC_SESSION_CONTROLLER_USER_INPUTS_H_
#define INC_SESSION_CONTROLLER_USER_INPUTS_H_

#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t  encoder_ticks;          // Signed → can count direction
    uint32_t encoder_switch_presses; // Count of encoder button presses
    uint32_t back_button_presses;
    uint32_t select_button_presses;
    bool     brake_button_active;    // Current pressed state
} user_input_states;

extern user_input_states interrupt_input_states;


void registerRotaryEncoderInput();
void registerRotaryEncoderSwInput();
void registerButtonBackInput();
void registerButtonSelectInput();
void registerButtonBrakeInput();


#ifdef __cplusplus
}
#endif

#endif /* INC_SESSION_CONTROLLER_USER_INPUTS_H_ */
