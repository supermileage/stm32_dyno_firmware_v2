#include "Tasks/SessionController/FiniteStateMachine.hpp"

FSM::FSM(osMessageQueueId_t sessionControllerToLumexLcdHandle) :
        _sessionControllerToLumexLcdHandle(sessionControllerToLumexLcdHandle), 
        _state{
            State::MainDynoState::IDLE,
            State::SettingsState::USB_LOGGING_OPTION_DISPLAYED
          },
        _usbLoggingEnabled(false),
        _sdLoggingEnabled(false),
        _fsmInputDataIndex(0)

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
        case MainDynoState::IDLE:
            break;
        case MainDynoState::SETTINGS_MENU:
            switch(_state.settingsState)
            {
                case SettingsState::USB_LOGGING_OPTION_DISPLAYED:
                    _state.settingsState = SettingsState::SD_LOGGING_OPTION_DISPLAYED;
                    break;
                case SettingsState::USB_LOGGING_OPTION_EDIT:
                    _usbLoggingEnabled = positiveTick;
                    break;
                case SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    _state.settingsState = SettingsState::PID_ENABLE_DISPLAYED;
                    break;
                case SettingsState::SD_LOGGING_OPTION_EDIT:
                   
                    break; 
                case SettingsState::PID_ENABLE_DISPLAYED:
                    _state.settingsState = SettingsState::USB_LOGGING_OPTION_DISPLAYED;
                    break;
                case SettingsState::PID_ENABLE_EDIT:
                    _state.settingsState = SettingsState::PID_ENABLE_DISPLAYED;
                    break;
                default:
                    break;
            }
            break;
        case MainDynoState::IN_SESSION:
            break;
    }

}
void FSM::HandleRotaryEncoderSwInput(void)
{
    switch(_state.mainState)
    {
        case MainDynoState::IDLE:
            break;
        case MainDynoState::SETTINGS_MENU:
            break;
        case MainDynoState::IN_SESSION:
            break;
    }

}
void FSM::HandleButtonBackInput(void)
{
    switch(_state.mainState)
    {
        case MainDynoState::IDLE:
            break;
        case MainDynoState::SETTINGS_MENU:
            _state.mainState = MainDynoState::IDLE;
            break;
        case MainDynoState::IN_SESSION:
            break;
    }

}
void FSM::HandleButtonSelectInput(void)
{
    switch(_state.mainState)
    {
        case MainDynoState::IDLE:
            _state.mainState = MainDynoState::SETTINGS_MENU;
            _state.settingsState = SettingsState::USB_LOGGING_OPTION_DISPLAYED;
            break;
        case MainDynoState::SETTINGS_MENU:
            switch(_state.settingsState)
            {
                case SettingsState::USB_LOGGING_OPTION_DISPLAYED:
                    _state.settingsState = SettingsState::USB_LOGGING_OPTION_EDIT;
                    break;
                case SettingsState::USB_LOGGING_OPTION_EDIT:
                    _state.settingsState = SettingsState::USB_LOGGING_OPTION_DISPLAYED;
                    break;
                case SettingsState::SD_LOGGING_OPTION_DISPLAYED:
                    _state.settingsState = SettingsState::SD_LOGGING_OPTION_EDIT;
                    break;
                case SettingsState::SD_LOGGING_OPTION_EDIT:
                    _state.settingsState = SettingsState::SD_LOGGING_OPTION_DISPLAYED;
                    break; 
                case SettingsState::PID_ENABLE_DISPLAYED:
                    _state.settingsState = SettingsState::PID_ENABLE_EDIT;
                    break;
                case SettingsState::PID_ENABLE_EDIT:
                    _state.settingsState = SettingsState::PID_ENABLE_DISPLAYED;
                    break;
                default:
                    break;
            }
            break;
        case MainDynoState::IN_SESSION:
            break;
    }
}


void FSM::HandleButtonBrakeInput(bool isEnabled)
{
    switch(_state.mainState)
    {
        case MainDynoState::IDLE:
            break;
        case MainDynoState::SETTINGS_MENU:
            break;
        case MainDynoState::IN_SESSION:
            break;
    }
    
}

State FSM::GetState() const 
{
    return _state;
}