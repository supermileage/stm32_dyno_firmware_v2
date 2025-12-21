#include <Tasks/SessionController/input_manager_interrupts.h>


// This data is used in both SessionController and this c file
volatile uint32_t interrupt_input_data_index = 0;

// Circular Buffer (only used in this c file)
volatile button_press_data button_press_circular_buffer[USER_INPUT_CIRCULAR_BUFFER_SIZE];

void register_rotary_encoder_input()
{
    // This function would be called on ROT_EN_A
    bool positive;
    // The state of ROT_EN_B would tell us the direction
    if (HAL_GPIO_ReadPin(ROT_EN_B_GPIO_Port, ROT_EN_B_Pin))
    {
        positive = false;
    }
    else
    {
        positive = true;
    }

    // Add the number of changed ticks to the buffer
    add_to_circular_buffer(ROT_EN_TICKS, positive);


}



void register_rotary_encoder_sw_input()
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

        // Add to the circular buffer for the session controller to read later
        // POSITIVE parameter does not matter here for the SELECT_INPUT. Read input_manager_interrupts.h file for explanation
        add_to_circular_buffer(ROT_EN_SW, false);

    }
}

void register_button_back_input()
{
    uint16_t current_time = __HAL_TIM_GET_COUNTER(buttonDebounceTimer);

    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

    __HAL_TIM_SET_COMPARE(buttonDebounceTimer, buttonDebounceTimerButtonBackTimerChannel, (current_time + BUTTON_DEBOUNCE_TIME_US) % UINT16_MAX);
    HAL_TIM_OC_Start_IT(buttonDebounceTimer, buttonDebounceTimerButtonBackTimerChannel);
}

void register_button_select_input()
{
    uint16_t current_time = __HAL_TIM_GET_COUNTER(buttonDebounceTimer);

    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

    __HAL_TIM_SET_COMPARE(buttonDebounceTimer, buttonDebounceTimerButtonSelectTimerChannel, (current_time + BUTTON_DEBOUNCE_TIME_US) % UINT16_MAX);
    HAL_TIM_OC_Start_IT(buttonDebounceTimer, buttonDebounceTimerButtonSelectTimerChannel);
}

void register_button_brake_input()
{
    uint16_t current_time = __HAL_TIM_GET_COUNTER(buttonDebounceTimer);

    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

    __HAL_TIM_SET_COMPARE(buttonDebounceTimer, buttonDebounceTimerButtonBrakeTimerChannel, (current_time + BUTTON_DEBOUNCE_TIME_US) % UINT16_MAX);
    HAL_TIM_OC_Start_IT(buttonDebounceTimer, buttonDebounceTimerButtonBrakeTimerChannel);
    
}

void register_button_back_debounce_timer_interrupt()
{
    HAL_TIM_OC_Stop_IT(buttonDebounceTimer, buttonDebounceTimerButtonBackTimerChannel);
    
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

        // Add to the circular buffer for the session controller to read later
        // POSITIVE parameter does not matter here for the SELECT_INPUT. Read input_manager_interrupts.h file for explanation
        add_to_circular_buffer(BTN_BACK, false);

    }

    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void register_button_select_debounce_timer_interrupt()
{
    HAL_TIM_OC_Stop_IT(buttonDebounceTimer, buttonDebounceTimerButtonSelectTimerChannel);
    
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

        // Add to the circular buffer for the session controller to read later
        // POSITIVE parameter does not matter here for the SELECT_INPUT. Read input_manager_interrupts.h file for explanation
        add_to_circular_buffer(BTN_SELECT, false);

    }

    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void register_button_brake_debounce_timer_interrupt()
{
    
    // Get the current pin state of the brake button
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin);
    if (pin_state == GPIO_PIN_RESET)
    {
        // Turn on LED
        HAL_GPIO_WritePin(LED_BRAKE_GPIO_Port, LED_BRAKE_Pin, GPIO_PIN_RESET);

        // Add to the circular buffer for the session controller to read later
        add_to_circular_buffer(BTN_BRAKE, true);
    }
    else
    {
        // Turn off LED
        HAL_GPIO_WritePin(LED_BRAKE_GPIO_Port, LED_BRAKE_Pin, GPIO_PIN_SET);

        // Add to the circular buffer for the session controller to read later
        add_to_circular_buffer(BTN_BRAKE, false);
    }

    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void add_to_circular_buffer(button_opcode opcode, bool positive)
{
    // Create the data struct to add
    button_press_data data_to_add;
    data_to_add.opcode = opcode;
    data_to_add.positive = positive;

    // Disable all button interrupts. Due to the nature of interrupts, they can interrupt each other
    // We cannot allow an interrupt to interrupt another while writing to the input_data buffer
    taskENTER_CRITICAL();

    // Critical section: write to the circular buffer
    button_press_circular_buffer[interrupt_input_data_index] = data_to_add;
    interrupt_input_data_index = (interrupt_input_data_index + 1) % USER_INPUT_CIRCULAR_BUFFER_SIZE;

    // renable the IRQs
    taskEXIT_CRITICAL();
}

volatile button_press_data* get_circular_buffer_data(uint32_t index)
{
    return &button_press_circular_buffer[index];
}



