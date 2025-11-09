#include <session_controller/user_inputs.h>

volatile user_input_states interrupt_input_states = {};



void registerRotaryEncoderInput()
{
    
}
void registerRotaryEncoderSwInput()
{
    // if button is pressed
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(ROT_EN_SW_GPIO_Port, ROT_EN_SW_Pin);
    if (pin_state = GPIO_PIN_RESET)
    {
        
    }
    else
    {

    }

}

void registerButtonBackInput()
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_BACK_GPIO_Port, BTN_BACK_Pin);
    if (pin_state = GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_BACK_GPIO_Port, LED_BACK_Pin, GPIO_PIN_RESET);
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_BACK_GPIO_Port, LED_BACK_Pin, GPIO_PIN_SET);
        interrupt_input_states.back_button_presses++;
    }
}

void registerButtonSelectInput()
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_SELECT_GPIO_Port, BTN_SELECT_Pin);
    if (pin_state = GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_SELECT_GPIO_Port, LED_SELECT_Pin, GPIO_PIN_RESET);
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_SELECT_GPIO_Port, LED_SELECT_Pin, GPIO_PIN_SET);
        interrupt_input_states.select_button_presses++;
    }
}

void registerButtonBrakeInput()
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin);
    if (pin_state = GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_BRAKE_GPIO_Port, LED_BRAKE_Pin, GPIO_PIN_RESET);
        interrupt_input_states.brake_button_active = true;
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_BRAKE_GPIO_Port, LED_BRAKE_Pin, GPIO_PIN_SET);
        interrupt_input_states.brake_button_active = false;
    }
}