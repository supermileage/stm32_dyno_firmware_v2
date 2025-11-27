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

typedef struct 
{
    osMessageQueueId_t usb_controller;
    osMessageQueueId_t sd_controller;
    osMessageQueueId_t force_sensor;
    osMessageQueueId_t optical_sensor;
    osMessageQueueId_t bpm_controller;
    osMessageQueueId_t pid_controller;
    osMessageQueueId_t lumex_lcd;
} session_controller_os_tasks;

void SessionController::Run()
{
    while(1)
    {
        // First Handle Any User Inputs
        _fsm.HandleUserInputs();

        // Get USB Enabled Status and enable USB Controller
        bool usbLoggingEnabled = _fsm.GetUSBLoggingEnabledStatus();
        // Only if the status has changed
        if (usbLoggingEnabled ^ _prevUSBLoggingEnabled)
        {
            osMessageQueuePut(task_queues->usb_controller, &usbLoggingEnabled, 0, 0);
            _prevUSBLoggingEnabled = usbLoggingEnabled;
        }

        // Get SD Card Enabled Status and enable SD Card Controller
        bool SDLoggingEnabled = _fsm.GetSDLoggingEnabledStatus();
        // Only if the status has changed
        if (SDLoggingEnabled ^ _prevSDLoggingEnabled)
        {
            osMessageQueuePut(task_queues->sd_controller, &SDLoggingEnabled, 0, 0);
            _prevSDLoggingEnabled = SDLoggingEnabled;
        }

        bool InSessionStatus = _fsm.GetInSessionStatus();

        bool InSessionRisingEdge = InSessionStatus && !_prevInSession;
        bool InSessionFallingEdge = !InSessionStatus && _prevInSession;
 

        // only run this code if the 'InSession' status has changed
        if (InSessionRisingEdge || InSessionFallingEdge)
        {
            session_controller_to_pid_controller pid_msg;
            
            // Get PID enabled status and enable PID Controller
            bool PIDEnabled = _fsm.GetPIDEnabledStatus();
            // Only if the status has changed
            if (PIDEnabled ^ _prevPIDEnabled)
            {
                pid_msg.enable_status = PIDEnabled;
                pid_msg.desired_rpm = _fsm.GetDesiredRpm();
                osMessageQueuePut(task_queues->pid_controller, &pid_msg, 0, 0);
                _prevPIDEnabled = PIDEnabled;
            }

            bool opticalEncoderEnable;
            bool forceSensorEnable;
            bool bpmEnable;

            // enable things
            if (InSessionRisingEdge)
            {
                opticalEncoderEnable = true;
                forceSensorEnable = true;
                bpmEnable = true;
            }
            
            // disable things
            else if (InSessionFallingEdge)
            {
                opticalEncoderEnable = false;
                forceSensorEnable = false;
                bpmEnable = false;
            }

            _prevInSession = InSessionStatus;





        }

        // Things that need to occur every pass of the loop
        if (InSessionStatus)
        {

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



