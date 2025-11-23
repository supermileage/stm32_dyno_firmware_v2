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



