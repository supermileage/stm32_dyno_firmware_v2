#include <SessionController/InputManagerInterrupts.h>

volatile user_input_states input_states = {};
volatile user_input_states prev_input_states = {};

volatile uint32_t input_data_index = 0;
volatile individual_input_data input_data[INPUT_CIRCULAR_BUFFER_SIZE];

void registerRotaryEncoderInput()
{
    // This function would be called on ROT_EN_A

    // The state of ROT_EN_B would tell us the direction
    if (HAL_GPIO_ReadPin(ROT_EN_B_GPIO_Port, ROT_EN_B_Pin))
    {
        input_states.encoder_ticks--;
    }
    else
    {
        input_states.encoder_ticks++;
    }

    // Add the number of changed ticks to the buffer
    addToCircularBuffer(ROT_EN_TICKS, input_states.encoder_ticks - prev_input_states.encoder_ticks);

    // store the previous state of encoder_ticks
    prev_input_states.encoder_ticks = input_states.encoder_ticks;

}
void registerRotaryEncoderSwInput()
{
    // if button is pressed
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(ROT_EN_SW_GPIO_Port, ROT_EN_SW_Pin);
    if (pin_state == GPIO_PIN_RESET)
    {
        // Do nothing, can change later
        __NOP();
    }
    else
    {
        // increment the number of back button presses
        input_states.encoder_switch_presses++;

        // Add to the circular buffer for the session controller to read later
        addToCircularBuffer(ROT_EN_SW, input_states.encoder_switch_presses - prev_input_states.encoder_switch_presses);

        // update the previous count
        prev_input_states.encoder_switch_presses = input_states.encoder_switch_presses;
    }

}

void registerButtonBackInput()
{
    // Get the current pin state of the back button
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_BACK_GPIO_Port, BTN_BACK_Pin);
    if (pin_state == GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_BACK_GPIO_Port, LED_BACK_Pin, GPIO_PIN_RESET);
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_BACK_GPIO_Port, LED_BACK_Pin, GPIO_PIN_SET);

        // increment the number of back button presses
        interrupt_input_states.back_button_presses++;

        // Add to the circular buffer for the session controller to read later
        addToCircularBuffer(BTN_BACK, input_states.back_button_presses - prev_input_states.back_button_presses);

        // update the previous count
        prev_input_states.back_button_presses = input_states.back_button_presses;
    }
}

void registerButtonSelectInput()
{
    // Get the current pin state of the select button
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_SELECT_GPIO_Port, BTN_SELECT_Pin);
    if (pin_state == GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_SELECT_GPIO_Port, LED_SELECT_Pin, GPIO_PIN_RESET);
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_SELECT_GPIO_Port, LED_SELECT_Pin, GPIO_PIN_SET);

        // increment the number of select button presses
        input_states.select_button_presses++;

        // Add to the circular buffer for the session controller to read later
        addToCircularBuffer(BTN_SELECT, input_states.select_button_presses - prev_input_states.select_button_presses);

        // update the previous count
        prev_input_states.select_button_presses = input_states.select_button_presses;
    }
}

void registerButtonBrakeInput()
{

    // Get the current pin state of the brake button
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin);
    if (pin_state == GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_BRAKE_GPIO_Port, LED_BRAKE_Pin, GPIO_PIN_RESET);

        // only add to buffer if the brake button status was true
        if (input_states.brake_button_active == false)
        {
            // change status to false
            input_states.brake_button_active = true;

            // Add to the circular buffer for the session controller to read later
            addToCircularBuffer(BTN_BRAKE, (int32_t) input_states.brake_button_active);
        }

        // prev input state of the brake button is not needed here
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_BRAKE_GPIO_Port, LED_BRAKE_Pin, GPIO_PIN_SET);

        // only add to buffer if the brake button status was true
        if (input_states.brake_button_active == true)
        {
            // change status to false
            input_states.brake_button_active = false;

            // Add to the circular buffer for the session controller to read later
            addToCircularBuffer(BTN_BRAKE, (int32_t) input_states.brake_button_active);
        }

        // prev input state of the brake button is not needed here
    }
}


void addToCircularBuffer(input_opcode opcode, int32_t count)
{
    // Create the data struct to add
    individual_input_data data_to_add;
    data_to_add.opcode = opcode;
    data_to_add.count = count;

    // Disable all button interrupts. Due to the nature of interrupts, they can interrupt each other
    // We cannot allow an interrupt to interrupt another while writing to the input_data buffer
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    HAL_NVIC_DisableIRQ(EXTI4_IRQn);
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

    // Critical section: write to the circular buffer
    input_data[input_data_index] = data_to_add;
    input_data_index = (input_data_index + 1) % INPUT_CIRCULAR_BUFFER_SIZE;

    // renable the IRQs
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

