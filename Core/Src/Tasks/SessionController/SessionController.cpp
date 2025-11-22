#include <Tasks/SessionController/SessionController.hpp>
#include <Tasks/SessionController/sessioncontroller_main.h>
#include <Tasks/SessionController/input_manager_interrupts.h>

SessionController::SessionController() : 
            _session_controller_input_data_index(0)
            {}

bool SessionController::Init(void)
{

    // The initial state of the brake button needs to be initialized if its pressed. If not, nothing needs to be done
    if (HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET)
    {
        HandleButtonBrakeInput(true);
    }
    
    return true;
}

void SessionController::Run()
{
    while(1)
    {
        // First Handle Any User Inputs
        HandleUserInputs();
    }
}

void SessionController::HandleUserInputs(void)
{
    
    while(_session_controller_input_data_index != interrupt_input_data_index)
    {
        volatile button_press_data* current_button_data = get_circular_buffer_data(_session_controller_input_data_index);
        switch(current_button_data->opcode)
        {
            case ROT_EN_TICKS:
                HandleRotaryEncoderInput(current_button_data->positive);
                break;
            case ROT_EN_SW:
                HandleRotaryEncoderSwInput();
                break;
            case BTN_BACK:
                HandleButtonBackInput();
                break;
            case BTN_SELECT:
                HandleButtonSelectInput();
                break;
            case BTN_BRAKE:
                HandleButtonBrakeInput(current_button_data->positive);
                break;
            default:
                break;
        }

        _session_controller_input_data_index = (_session_controller_input_data_index + 1) % USER_INPUT_CIRCULAR_BUFFER_SIZE;
    }
}

void SessionController::HandleRotaryEncoderInput(bool positiveTick)
{
    

}
void SessionController::HandleRotaryEncoderSwInput(void)
{
    

}
void SessionController::HandleButtonBackInput(void)
{
    

}
void SessionController::HandleButtonSelectInput(void)
{
    
}
void SessionController::HandleButtonBrakeInput(bool isEnabled)
{

    
}


