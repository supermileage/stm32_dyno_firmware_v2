#ifndef INC_SESSION_CONTROLLER_FSM_H_
#define INC_SESSION_CONTROLLER_FSM_H_

#include <cstdint.h>
#include "cmsis_os2.h"

#include "input_manager_interrupts.h"

// struct with info for the state
struct State
{
    enum class MainDynoState
    {
        IDLE,
        SETTINGS_MENU,
        IN_SESSION
    };

    enum class SettingsState
    {
        USB_LOGGING_OPTION_DISPLAYED,
        USB_LOGGING_OPTION_EDIT,
        SD_LOGGING_OPTION_DISPLAYED,
        SD_LOGGING_OPTION_EDIT,
        PID_ENABLE_DISPLAYED,
        PID_ENABLE_EDIT,
    };

    MainDynoState mainState;
    SettingsState settingsState;
};


class FSM
{
public:
    FSM(osMessageQueueId_t sessionControllerToLumexLcdHandle);
    void HandleUserInputs();
private:

    
    void HandleRotaryEncoderInput(bool positiveTick);
    void HandleRotaryEncoderSwInput();
    void HandleButtonBackInput();
    void HandleButtonSelectInput();
    void HandleButtonBrakeInput(bool isEnabled);

    State GetState() const;

    osMessageQueueId_t _sessionControllerToLumexLcdHandle;

    State _state;
    bool _usbLoggingEnabled;
    bool _sdLoggingEnabled;
    uint32_t _fsmInputDataIndex;
}

#endif // INC_SESSION_CONTROLLER_FSM_H_

