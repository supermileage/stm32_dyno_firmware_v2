#include "Tasks/SessionController/FiniteStateMachine.hpp"

FSM::FSM(osMessageQueueId_t sessionControllerToLumexLcdHandle) :
        _sessionControllerToLumexLcdHandle(sessionControllerToLumexLcdHandle), 
        _state{
            State::State::MainDynoState::IDLE,
            State::State::SettingsState::USB_LOGGING_OPTION_DISPLAYED
          },
        _usbLoggingEnabled(false),
        _sdLoggingEnabled(false),
        _pidOptionToggleableEnabled(false),
        _inSession(false),
        _desiredBpmDutyCycle(0),
        _desiredRpm(5000),
        _desiredRpmIncrement(0),
        _fsmInputDataIndex(0)
        {
            ClearDisplay();
            IdleState();

            // The initial state of the brake button needs to be initialized if its pressed. If not, nothing needs to be done
            // if (HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port, BTN_BRAKE_Pin) == GPIO_PIN_RESET)
            // {
            //     HandleButtonBrakeInput(true);
            // }

        }

template<typename T>
static inline T clamp(T x, T minVal, T maxVal)
{
    if (x < minVal) return minVal;
    if (x > maxVal) return maxVal;
    return x;
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
                    if (positiveTick) SDLoggingOptionDisplayedSettingsState();
                    else PIDDesiredRPMOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::USB_LOGGING_OPTION_EDIT:
                    // _usbLoggingEnabled = positiveTick;
                    // USBLoggingOptionEditSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    if (positiveTick) PIDOptionDisplayedSettingsState();
                    else USBLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_EDIT:
                    // _sdLoggingEnabled = positiveTick;
                    // SDLoggingOptionEditSettingsState();
                    break; 
                case State::SettingsState::PID_ENABLE_DISPLAYED:
                    if (positiveTick) PIDDesiredRPMOptionDisplayedSettingsState();
                    else SDLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::PID_ENABLE_EDIT:
                    // _pidOptionToggleableEnabled = positiveTick;
                    // PIDOptionEditSettingsState();
                    break;
                case State::SettingsState::PID_DESIRED_RPM_DISPLAYED:
                    if (positiveTick) USBLoggingOptionDisplayedSettingsState();
                    else PIDOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::PID_DESIRED_RPM_EDIT:
                    ConvertUserInputIntoDesiredRpm(positiveTick);
                    PIDDesiredRPMOptionEditSettingsState(false);
                    break;
                default:
                    break;
            }
            break;
        case State::MainDynoState::IN_SESSION:
            if (_pidOptionToggleableEnabled)
            {
                float increment = 0.01f;
                if (positiveTick)
                {
                    increment *= -1;
                }

                _desiredBpmDutyCycle = clamp(_desiredBpmDutyCycle + increment, 0.0f, 1.0f);
            }
            break;
    }

}
void FSM::HandleRotaryEncoderSwInput(void)
{

    _inSession = true;
    InSessionState();
    
    
    // switch(_state.mainState)
    // {
    //     case State::MainDynoState::IDLE:
    //         InSessionState();
    //         break;
    //     case State::MainDynoState::SETTINGS_MENU:
    //         InSessionState();
    //         // switch(_state.settingsState)
    //         // {
    //         //     case State::SettingsState::PID_DESIRED_RPM_EDIT:
    //         //         _desiredRpmIncrement++;
    //         //         _desiredRpmIncrement %= 5;
    //         //         break;
    //         //     default:
    //         //         break;
    //         // }
    //         break;
    //     case State::MainDynoState::IN_SESSION:
    //         break;
    // }

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
                    IdleState();
                    break;
                case State::SettingsState::USB_LOGGING_OPTION_EDIT:
                    USBLoggingOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    IdleState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_EDIT:
                    SDLoggingOptionDisplayedSettingsState();
                    break; 
                case State::SettingsState::PID_ENABLE_DISPLAYED:
                    IdleState();
                    break;
                case State::SettingsState::PID_ENABLE_EDIT:
                    PIDOptionDisplayedSettingsState();
                    break;
                case State::SettingsState::PID_DESIRED_RPM_DISPLAYED:
                    IdleState();
                    break;
                case State::SettingsState::PID_DESIRED_RPM_EDIT:
                    PIDDesiredRPMOptionDisplayedSettingsState();
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
                    _usbLoggingEnabled = !_usbLoggingEnabled;
                    USBLoggingOptionEditSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    SDLoggingOptionEditSettingsState();
                    break;
                case State::SettingsState::SD_LOGGING_OPTION_EDIT:
                    _sdLoggingEnabled = !_sdLoggingEnabled;
                    SDLoggingOptionEditSettingsState();
                    break; 
                case State::SettingsState::PID_ENABLE_DISPLAYED:
                    PIDOptionEditSettingsState();
                    break;
                case State::SettingsState::PID_ENABLE_EDIT:
                    _pidOptionToggleableEnabled = !_pidOptionToggleableEnabled;
                    PIDOptionEditSettingsState();
                    break;
                case State::SettingsState::PID_DESIRED_RPM_DISPLAYED:
                    PIDDesiredRPMOptionEditSettingsState(true);
                    break;
                case State::SettingsState::PID_DESIRED_RPM_EDIT:
                    PIDDesiredRPMOptionDisplayedSettingsState();
                    break;
                default:
                    break;
            }
            break;
        case State::MainDynoState::IN_SESSION:
            _pidEnabled = (_pidOptionToggleableEnabled) ? !_pidEnabled : _pidEnabled;
            break;
    }
}


void FSM::HandleButtonBrakeInput(bool isEnabled)
{
    // if (isEnabled)
    // {
    //     _inSession = true;
    //     InSessionState();
    // }
    // else
    // {
    //     _inSession = false;
    //     IdleState();
    // }
    
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

void FSM::ConvertUserInputIntoDesiredRpm(bool positiveTick)
{
    int increment = 0;
    switch (_desiredRpmIncrement)
    {
        case 0:
            increment = 10000;
            break;
        case 1:
            increment = 1000;
            break;
        case 2:
            increment = 100;
            break;
        case 3:
            increment = 10;
            break;
        case 4:
            increment = 1;
            break;
        default:
            break;
    }

    if (!positiveTick)
    {
        increment *= -1;
    }


    _desiredRpm = std::max(0, _desiredRpm + increment);

}

void FSM::ClearDisplay()
{

    AddToLumexLCDMessageQueue(CLEAR_DISPLAY, 0, 0, NULL, 0);

}

void FSM::IdleState()
{
    _state.mainState = State::MainDynoState::IDLE;

    ClearDisplay();
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 6, "DYNO", sizeof("DYNO") - 1);
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 2, "PRESS SELECT", sizeof("PRESS SELECT") - 1);
}

void FSM::USBLoggingOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::USB_LOGGING_OPTION_DISPLAYED;

    ClearDisplay();
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 2, "USB LOGGING", sizeof("USB LOGGING") - 1);

    if (_usbLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "ENABLED", sizeof("ENABLED") - 1);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "DISABLED", sizeof("DISABLED") - 1);
}   

void FSM::USBLoggingOptionEditSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::USB_LOGGING_OPTION_EDIT;

    ClearDisplay();
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 2, "USB LOGGING", sizeof("USB LOGGING") - 1);

    if (_usbLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "ENABLED", sizeof("ENABLED") - 1);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "DISABLED", sizeof("DISABLED") - 1);
}

void FSM::SDLoggingOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::SD_LOGGING_OPTION_DISPLAYED;

    ClearDisplay();

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 3, "SD LOGGING", sizeof("SD LOGGING") - 1);
    
    if (_sdLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "ENABLED", sizeof("ENABLED") - 1);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "DISABLED", sizeof("DISABLED") - 1);
}

void FSM::SDLoggingOptionEditSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::SD_LOGGING_OPTION_EDIT;

    ClearDisplay();
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 3, "SD LOGGING", sizeof("SD LOGGING") - 1);

    if (_sdLoggingEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "ENABLED", sizeof("ENABLED") - 1);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "DISABLED", sizeof("DISABLED") - 1);
}

void FSM::PIDOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::PID_ENABLE_DISPLAYED;

    ClearDisplay();
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 2, "PID LOGGING", sizeof("PID LOGGING") - 1);

    if (_pidOptionToggleableEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "ENABLED", sizeof("ENABLED") - 1);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "DISABLED", sizeof("DISABLED") - 1);
}

void FSM::PIDOptionEditSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::PID_ENABLE_EDIT;

    ClearDisplay();
    
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 2, "PID LOGGING", sizeof("PID LOGGING") - 1);

    if (_pidOptionToggleableEnabled)
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "ENABLED", sizeof("ENABLED") - 1);
    else
        AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 4, "DISABLED", sizeof("DISABLED") - 1);
}

void FSM::PIDDesiredRPMOptionDisplayedSettingsState()
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::PID_DESIRED_RPM_DISPLAYED;

    ClearDisplay();

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY,  0, 2, "PID DES RPM1", sizeof("PID DES RPM1") - 1);

}

void FSM::PIDDesiredRPMOptionEditSettingsState(bool clearDisplay)
{
    _state.mainState = State::MainDynoState::SETTINGS_MENU;
    _state.settingsState = State::SettingsState::PID_DESIRED_RPM_EDIT;

    if (clearDisplay) ClearDisplay();

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 2, "PID DES RPM2", sizeof("PID DES RPM2") - 1);

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%5d", static_cast<int>(_desiredRpm));

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 5, buffer, sizeof(buffer) - 1);
}



void FSM::InSessionState()
{
    _state.mainState = State::MainDynoState::IN_SESSION;

    ClearDisplay();
    
    // The actual data will be managed in the session controller
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 0, "n:     0 T: 0.00", sizeof("n:     0 T: 0.00") - 1);
    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 0, "P:     0.00", sizeof("P:     0.00") - 1);
}

void FSM::DisplayRpm(float rpm)
{
    char buf[6]; // Enough for 32-bit integers
    uint32_t value = static_cast<uint32_t>(std::round(rpm));
    snprintf(buf, sizeof(buf), "%5lu", value);

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 3, buf, sizeof(buf) - 1);
}

void FSM::DisplayTorque(float torque)
{
    char buf[6]; // Enough for torque with 2 decimals
    float value = std::round(torque * 100.0) / 100.0;
    snprintf(buf, sizeof(buf), "%5.2f", value);

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 0, 12, buf, sizeof(buf) - 1);
}

void FSM::DisplayPower(float power)
{
    char buf[9]; // Enough for big power numbers with 2 decimals
    float value = std::round(power * 100.0) / 100.0;
    snprintf(buf, sizeof(buf), "%8.2f", value);

    AddToLumexLCDMessageQueue(WRITE_TO_DISPLAY, 1, 3, buf, sizeof(buf) - 1);
}



void FSM::AddToLumexLCDMessageQueue(session_controller_to_lumex_lcd_opcode opcode, uint8_t row, uint8_t column, const char* display_string, size_t size)
{

	session_controller_to_lumex_lcd msg;
    msg.op = opcode;
    msg.row = row;
    msg.column = column;
    msg.size = size;

    strncpy(msg.display_string, display_string, sizeof(msg.display_string) - 1);
    msg.display_string[sizeof(msg.display_string) - 1] = '\0'; // Ensure null termination

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

float FSM::GetDesiredBpmDutyCycle() const
{
    return _pidOptionToggleableEnabled ? _desiredBpmDutyCycle : -1;
}

float FSM::GetDesiredRpm() const
{
    return _desiredRpm;
}

float FSM::GetDesiredAngularVelocity() const
{
    return _desiredRpm * 2 * M_PI / 60;
}
