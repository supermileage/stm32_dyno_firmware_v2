#ifndef INC_SESSION_CONTROLLER_FSM_H_
#define INC_SESSION_CONTROLLER_FSM_H_

#include <algorithm>

#include <cstdint>
#include <cstdio>

#include "cmsis_os2.h"

#include "MessagePassing/messages.h"

#include "input_manager_interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

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
        PID_DESIRED_RPM_DISPLAYED,
        PID_DESIRED_RPM_EDIT
    };

    MainDynoState mainState;
    SettingsState settingsState;
};


class FSM
{
public:
    FSM(osMessageQueueId_t sessionControllerToLumexLcdHandle);
    void HandleUserInputs();

    // Display related methods
    void ClearDisplay();
    void AddToLumexLCDMessageQueue(session_controller_to_lumex_lcd_opcode opcode, const char* display_string, uint8_t row, uint8_t column);
    
    // Getters
    State GetState() const;
    bool GetUSBLoggingEnabledStatus() const;
    bool GetSDLoggingEnabledStatus() const;
    bool GetPIDEnabledStatus() const;
    bool GetInSessionStatus() const;

    float GetDesiredBpmDutyCycle() const;

    int GetDesiredRpm() const;

    void DisplayRpm(float rpm);
    void DisplayTorque(float torque);
    void DisplayPower(float power);

private:
    // Methods which are called when a state change is done
    void IdleState();
    void USBLoggingOptionDisplayedSettingsState(); 
    void USBLoggingOptionEditSettingsState();
    void SDLoggingOptionDisplayedSettingsState();
    void SDLoggingOptionEditSettingsState();   
    void PIDOptionDisplayedSettingsState();
    void PIDOptionEditSettingsState();
    void PIDDesiredRPMOptionDisplayedSettingsState();
    void PIDDesiredRPMOptionEditSettingsState(bool clearDisplay);
    void InSessionState();
    
    // user input handlers
    void HandleRotaryEncoderInput(bool positiveTick);
    void HandleRotaryEncoderSwInput();
    void HandleButtonBackInput();
    void HandleButtonSelectInput();
    void HandleButtonBrakeInput(bool isEnabled);

    void ConvertUserInputIntoDesiredRpm(bool positiveTick);

    osMessageQueueId_t _sessionControllerToLumexLcdHandle;

    State _state;
    
    
    bool _usbLoggingEnabled;
    bool _sdLoggingEnabled;
    bool _pidOptionToggleableEnabled;
    bool _pidEnabled;
    bool _inSession;
    float _desiredBpmDutyCycle;
    int _desiredRpm;
    int _desiredRpmIncrement;
    uint32_t _fsmInputDataIndex;
};


#ifdef __cplusplus
}
#endif





#endif // INC_SESSION_CONTROLLER_FSM_H_

