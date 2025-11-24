#include "Tasks/SessionController/FiniteStateMachine.hpp"

FSM::FSM(osMessageQueueId_t sessionControllerToLumexLcdHandle) :
        _sessionControllerToLumexLcdHandle(sessionControllerToLumexLcdHandle), 
        _state{
            State::State::MainDynoState::IDLE,
            State::State::SettingsState::USB_LOGGING_OPTION_DISPLAYED
          },
        _usbLoggingEnabled(false),
        _sdLoggingEnabled(false),
        _pidEnabled(false),
        _inSession(false),
        _fsmInputDataIndex(0)
        {
            ClearDisplay();
            IdleState();

            // The initial state of the brake button needs to be initialized if its pressed. If not, nothing needs to be done
            if (HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET)
            {
                HandleButtonBrakeInput(true);
            }

        }

void FSM::HandleUserInputs(void)
{
    
    while(_fsmInputDataIndex != interrupt_input_data_index)
    {
        volatile button_press_data* current_button_data = get_circular_buffer_data(_fsmInputDataIndex);
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

        _fsmInputDataIndex = (_fsmInputDataIndex + 1) % USER_INPUT_CIRCULAR_BUFFER_SIZE;
    }
}

void FSM::HandleRotaryEncoderInput(bool positiveTick)
{
    switch(_state.mainState)
    {
        case State::MainDynoState::IDLE:
            break;
        case State::MainDynoState::SETTINGS_MENU:
            switch(_state.settingsState)
            {
                case State::SettingsState::USB_LOGGING_OPTION_DISPLAYED:
                    SDLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::USB_LOGGING_OPTION_EDIT:
                    _usbLoggingEnabled = positiveTick;
                    USBLoggingOptionEditSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    PIDOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_EDIT:
                    _sdLoggingEnabled = positiveTick;
                    SDLoggingOptionEditSettingsState();
                    break; 
                case State::SettingsState::PID_ENABLE_DISPLAYED:
                    USBLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::PID_ENABLE_EDIT:
                    _pidEnabled = positiveTick;
                    PIDOptionEditSettingsState();
                    break;
                default:
                    break;
            }
            break;
        case State::MainDynoState::IN_SESSION:
            break;
    }

}
void FSM::HandleRotaryEncoderSwInput(void)
{
    switch(_state.mainState)
    {
        case State::MainDynoState::IDLE:
            break;
        case State::MainDynoState::SETTINGS_MENU:
            break;
        case State::MainDynoState::IN_SESSION:
            break;
    }

}
void FSM::HandleButtonBackInput(void)
{
    switch(_state.mainState)
    {
        case State::MainDynoState::IDLE:
            break;
        case State::MainDynoState::SETTINGS_MENU:
            switch(_state.settingsState)
            {
                case State::SettingsState::USB_LOGGING_OPTION_DISPLAYED:
                    _inSession = false;
                    IdleState();
                    break;
                case State::SettingsState::USB_LOGGING_OPTION_EDIT:
                    USBLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    _inSession = false;
                    IdleState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_EDIT:
                    SDLoggingOptionDisplayedSettingsState();
                    break; 
                case State::SettingsState::PID_ENABLE_DISPLAYED:
                    _inSession = false;
                    IdleState();
                    break;
                case State::SettingsState::PID_ENABLE_EDIT:
                    PIDOptionDisplayedSettingsState();
                    break;
                default:
                    break;
            }
            break;
        case State::MainDynoState::IN_SESSION:
            _inSession = false;
            IdleState();
            break;
    }

}
void FSM::HandleButtonSelectInput(void)
{
    switch(_state.mainState)
    {
        case State::MainDynoState::IDLE:
            USBLoggingOptionDisplayedSettingsState();
            break;
        case State::MainDynoState::SETTINGS_MENU:
            switch(_state.settingsState)
            {
                case State::SettingsState::USB_LOGGING_OPTION_DISPLAYED:
                    USBLoggingOptionEditSettingsState();
                    break;
                case State::SettingsState::USB_LOGGING_OPTION_EDIT:
                    USBLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    SDLoggingOptionEditSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_EDIT:
                    SDLoggingOptionDisplayedSettingsState();
                    break; 
                case State::SettingsState::PID_ENABLE_DISPLAYED:
                    PIDOptionEditSettingsState();
                    break;
                case State::SettingsState::PID_ENABLE_EDIT:
                    PIDOptionDisplayedSettingsState();
                    break;
                default:
                    break;
            }
            break;
        case State::MainDynoState::IN_SESSION:
            break;
    }
}


void FSM::HandleButtonBrakeInput(bool isEnabled)
{
    if (isEnabled)
    {
        _inSession = true;
        InSessionState();
    }
    else
    {
        _inSession = false;
        IdleState();
    }
    
    // switch(_state.mainState)
    // {
    //     case State::MainDynoState::IDLE:
    //         break;
    //     case State::MainDynoState::SETTINGS_MENU:
    //         _inSession = true;
    //         DisplayInSessionScreen();
    //         break;
    //     case State::MainDynoState::IN_SESSION:
    //         break;
    // }
    
}

void FSM::ClearDisplay()
{

    AddToLumexLCDMessageQueue(CLEAR_DISPLAY, NULL, 0, 0);

}

void FSM::IdleState()
{
    _state.mainState = State::MainDynoState::IDLE;
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DYNO" , 0, 6);
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "PRESS SELECT", 1, 2);
}

void FSM::USBLoggingOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::USB_LOGGING_OPTION_DISPLAYED;
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "USB LOGGING", 0, 2);

    if (_usbLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "ENABLED" , 1, 4);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DISABLED" , 1, 4);
}   

void FSM::USBLoggingOptionEditSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::USB_LOGGING_OPTION_EDIT;
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "USB LOGGING", 0, 2);

    if (_usbLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "ENABLED" , 1, 4);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DISABLED" , 1, 4);
}

void FSM::SDLoggingOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::SD_LOGGING_OPTION_DISPLAYED;

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "SD LOGGING", 0, 3);
    
    if (_sdLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "ENABLED" , 1, 4);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DISABLED" , 1, 4);
}

void FSM::SDLoggingOptionEditSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::SD_LOGGING_OPTION_EDIT;
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "SD LOGGING", 0, 3);

    if (_sdLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "ENABLED" , 1, 4);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DISABLED" , 1, 4);
}

void FSM::PIDOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::PID_ENABLE_DISPLAYED;
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "PID LOGGING", 0, 2);

    if (_pidEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "ENABLED" , 1, 4);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DISABLED" , 1, 4);
}

void FSM::PIDOptionEditSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::PID_ENABLE_EDIT;
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "PID LOGGING", 0, 2);

    if (_pidEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "ENABLED" , 1, 4);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "DISABLED" , 1, 4);
}

void FSM::InSessionState()
{
    _state.mainState = State::MainDynoState::IN_SESSION;
    
    // The actual data will be managed in the session controller
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "n: 00000 F: 00.0" , 0, 0);
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, (char*) "P: 00000.00" , 1, 0);
}

void FSM::AddToLumexLCDMessageQueue(session_controller_to_lumex_lcd_opcode opcode, const char* display_string, uint8_t row, uint8_t column)
{
    session_controller_to_lumex_lcd msg;
    msg.op = opcode;
    msg.display_string = display_string;
    msg.row = row;
    msg.column = column;

    osMessageQueuePut(_sessionControllerToLumexLcdHandle, &msg, 0, 0);
}

State FSM::GetState() const 
{
    return _state;
}

bool FSM::GetUSBLoggingEnabledStatus() const
{
    return _usbLoggingEnabled;
}

bool FSM::GetSDLoggingEnabledStatus() const
{
    return _sdLoggingEnabled;
}

bool FSM::GetPIDEnabledStatus() const
{
    return _pidEnabled;
}

bool FSM::GetInSessionStatus() const
{
    return _state.mainState == State::MainDynoState::IN_SESSION;
}