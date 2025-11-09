#include <session_controller/session_controller.h>

class SessionController
{
    public:
        SessionController();
        ~SessionController() = default;

        bool Init(void);
        void Run(void);

    private:
        void HandleUserInputs(void);
        void HandleRotaryEncoderInput();
        void HandleRotaryEncoderSwInput();
        void HandleButtonBackInput();
        void HandleButtonSelectInput();
        void HandleButtonBrakeInput();

        user_input_states _sc_input_states;
};

SessionController::SessionController(){}

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
    // if the session controller state of the rotary encoder ticks is the same as the interrupt copy, the encoder has not been touched
    if (_sc_input_states.encoder_ticks != interrupt_input_states.encoder_ticks)
    {
        return;
    }

}
void SessionController::HandleRotaryEncoderSwInput(void)
{
    // if the session controller button press count of the rotary encoder switch is the same as the interrupt copy, the encoder switch has not been touched
    if (_sc_input_states.encoder_switch_presses != interrupt_input_states.encoder_switch_presses)
    {
        return;
    }
}
void SessionController::HandleButtonBackInput(void)
{
    // if the session controller button press count of the back button is the same as the interrupt copy, the back button has not been touched
    if (_sc_input_states.back_button_presses != interrupt_input_states.back_button_presses)
    {
        return;
    }
}
void SessionController::HandleButtonSelectInput(void)
{
    // if the session controller button press count of the select button is the same as the interrupt copy, the select button has not been touched
    if (_sc_input_states.select_button_presses != interrupt_input_states.select_button_presses)
    {
        return;
    }
}
void SessionController::HandleButtonBrakeInput(void)
{
    // if the session controller button press state of the brake button is the same as the interrupt copy, the brake button has not been touched
    if (_sc_input_states.brake_button_active != interrupt_input_states.brake_button_active)
    {
        return;
    }

    
}

bool SessionController::Init(void)
{
    // Button Interrupt struct
    interrupt_input_states.encoder_ticks = 0;
    interrupt_input_states.encoder_switch_presses = 0;
    interrupt_input_states.back_button_presses = 0;
    interrupt_input_states.select_button_presses = 0;
    interrupt_input_states.brake_button_active = HAL_GPIO_WritePin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET;

    interrupt_input_states.encoder_ticks = 0;
    interrupt_input_states.encoder_switch_presses = 0;
    interrupt_input_states.back_button_presses = 0;
    interrupt_input_states.select_button_presses = 0;
    interrupt_input_states.brake_button_active = HAL_GPIO_WritePin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET;
    
    return true;
}