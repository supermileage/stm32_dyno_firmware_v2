#include <Tasks/SessionController/SessionController.hpp>

SessionController::SessionController(osMessageQueueId_t sessionControllerToLumexLcdHandle) : 
                _fsm(sessionControllerToLumexLcdHandle),
                _sessionControllerToLumexLcdHandle(sessionControllerToLumexLcdHandle),
                _prevUSBLoggingEnabled(false),
                _prevSDLoggingEnabled(false),
                _prevPIDEnabled(false)
            {}

bool SessionController::Init(void)
{
    
    return true;
}

void SessionController::Run()
{
    while(1)
    {
        // First Handle Any User Inputs
        _fsm.HandleUserInputs();

        // Get USB Enabled Status
        bool usbLoggingEnabled = _fsm.GetUSBLoggingEnabledStatus();
        if (usbLoggingEnabled && !_prevUSBLoggingEnabled)
        {
            _prevUSBLoggingEnabled = usbLoggingEnabled;
        }
        else if (!usbLoggingEnabled && _prevUSBLoggingEnabled)
        {
            _prevUSBLoggingEnabled = usbLoggingEnabled;
        }

        // Get SD Card Enabled Status
        bool SDLoggingEnabled = _fsm.GetSDLoggingEnabledStatus();
        if (SDLoggingEnabled && !_prevSDLoggingEnabled)
        {
            _prevSDLoggingEnabled = SDLoggingEnabled;
        }
        else if (!SDLoggingEnabled && _prevSDLoggingEnabled)
        {
            _prevSDLoggingEnabled = SDLoggingEnabled;
        }

        // Get PID enabled status
        bool PIDEnabled = _fsm.GetPIDEnabledStatus();
        if (PIDEnabled && !_prevPIDEnabled)
        {
            _prevPIDEnabled = PIDEnabled;
        }
        else if (!PIDEnabled && _prevSDLoggingEnabled)
        {
            _prevPIDEnabled = PIDEnabled;
        }

        if (_fsm.GetInSessionStatus())
        {
            
        }
    }
}

extern "C" void sessioncontroller_main(osMessageQueueId_t sessionControllerToLumexLcdHandle)
{
    SessionController controller = SessionController(sessionControllerToLumexLcdHandle);

	if (!controller.Init())
	{
		return;
	}


	controller.Run();
}



