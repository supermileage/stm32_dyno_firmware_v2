#include <SessionController/SessionController.h>

SessionController::SessionController(){}

bool SessionController::Init(void)
{
    // Button Interrupt struct
    interrupt_input_states.encoder_ticks = 0;
    interrupt_input_states.encoder_switch_presses = 0;
    interrupt_input_states.back_button_presses = 0;
    interrupt_input_states.select_button_presses = 0;
    interrupt_input_states.brake_button_active = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET;

    interrupt_input_states.encoder_ticks = 0;
    interrupt_input_states.encoder_switch_presses = 0;
    interrupt_input_states.back_button_presses = 0;
    interrupt_input_states.select_button_presses = 0;
    interrupt_input_states.brake_button_active = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET;
    
    return true;
}

void SessionController::Run()
{
    while(1)
    {
        
    }
}

void SessionController::HandleUserInputs(void)
{
    HandleRotaryEncoderInput();
    HandleRotaryEncoderSwInput();
    HandleButtonBackInput();
    HandleButtonSelectInput();
    HandleButtonBrakeInput();
}

void SessionController::HandleRotaryEncoderInput(void)
{
    

}
void SessionController::HandleRotaryEncoderSwInput(void)
{
    
    // if the session controller button press count of the rotary encoder switch is the same as the interrupt copy, the encoder switch has not been touched
    if (_sc_input_states.encoder_switch_presses != encoder_switch_presses_copy)
    {
        return;
    }
}
void SessionController::HandleButtonBackInput(void)
{
    
    // if the session controller button press count of the back button is the same as the interrupt copy, the back button has not been touched
    if (_sc_input_states.back_button_presses != back_button_presses_copy)
    {
        return;
    }
}
void SessionController::HandleButtonSelectInput(void)
{
    
    // if the session controller button press count of the select button is the same as the interrupt copy, the select button has not been touched
    if (_sc_input_states.select_button_presses != select_button_presses_copy)
    {
        return;
    }
}
void SessionController::HandleButtonBrakeInput(void)
{

    // if the session controller button press state of the brake button is the same as the interrupt copy, the brake button has not been touched
    if (_sc_input_states.brake_button_active != brake_button_active_copy)
    {
        return;
    }

    
}


