#include <Tasks/SessionController/SessionController.hpp>

SessionController::SessionController(session_controller_os_tasks* task_queues) : 
                _fsm(task_queues->lumex_lcd),
                _task_queues(task_queues),
                _prevUSBLoggingEnabled(false),
                _prevSDLoggingEnabled(false),
                _prevPIDEnabled(false),
                _prevInSession(false)
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

        // Get USB Enabled Status and enable USB Controller
        bool usbLoggingEnabled = _fsm.GetUSBLoggingEnabledStatus();
        if (usbLoggingEnabled && !_prevUSBLoggingEnabled)
        {
            _prevUSBLoggingEnabled = usbLoggingEnabled;
        }
        else if (!usbLoggingEnabled && _prevUSBLoggingEnabled)
        {
            _prevUSBLoggingEnabled = usbLoggingEnabled;
        }

        // Get SD Card Enabled Status and enable SD Card Controller
        bool SDLoggingEnabled = _fsm.GetSDLoggingEnabledStatus();
        if (SDLoggingEnabled && !_prevSDLoggingEnabled)
        {
            _prevSDLoggingEnabled = SDLoggingEnabled;
        }
        else if (!SDLoggingEnabled && _prevSDLoggingEnabled)
        {
            _prevSDLoggingEnabled = SDLoggingEnabled;
        }

        bool InSessionStatus = _fsm.GetInSessionStatus();

        
        // InSession looping forever
        if (InSessionStatus)
        {
            // only run this code if the 'InSession' status has changed
            if (InSessionStatus != _prevInSession)
            {
                // Get PID enabled status and enable PID Controller
                bool PIDEnabled = _fsm.GetPIDEnabledStatus();
                if (PIDEnabled && !_prevPIDEnabled)
                {
                    _prevPIDEnabled = PIDEnabled;
                }
                else if (!PIDEnabled && _prevSDLoggingEnabled)
                {
                    _prevPIDEnabled = PIDEnabled;
                }


            }
            

            
        }
    }
}

extern "C" void sessioncontroller_main(session_controller_os_tasks* task_queues)
{
    SessionController controller = SessionController(task_queues);

	if (!controller.Init())
	{
		return;
	}


	controller.Run();
}



