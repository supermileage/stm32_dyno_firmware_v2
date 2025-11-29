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
        // Only if the status has changed
        if (usbLoggingEnabled ^ _prevUSBLoggingEnabled)
        {
            osMessageQueuePut(_task_queues->usb_controller, &usbLoggingEnabled, 0, 0);
            _prevUSBLoggingEnabled = usbLoggingEnabled;
        }

        // Get SD Card Enabled Status and enable SD Card Controller
        bool SDLoggingEnabled = _fsm.GetSDLoggingEnabledStatus();
        // Only if the status has changed
        if (SDLoggingEnabled ^ _prevSDLoggingEnabled)
        {
            osMessageQueuePut(_task_queues->sd_controller, &SDLoggingEnabled, 0, 0);
            _prevSDLoggingEnabled = SDLoggingEnabled;
        }

        bool InSessionStatus = _fsm.GetInSessionStatus();

        bool InSessionRisingEdge = InSessionStatus && !_prevInSession;
        bool InSessionFallingEdge = !InSessionStatus && _prevInSession;
 

        // only run this code if the 'InSession' status has changed
        if (InSessionRisingEdge || InSessionFallingEdge)
        {
            bool opticalEncoderEnable;
            bool forceSensorEnable;

            // enable things
            if (InSessionRisingEdge)
            {
                opticalEncoderEnable = true;
                forceSensorEnable = true;

            }
            
            // disable things
            else if (InSessionFallingEdge)
            {
                session_controller_to_bpm bpmSettings;

                opticalEncoderEnable = false;
                forceSensorEnable = false;

                bpmSettings.op = STOP_PWM;
                bpmSettings.new_duty_cycle_percent = static_cast<float>(0);

                osMessageQueuePut(_task_queues->bpm_controller, &bpmSettings, 0, osWaitForever);
                
            }

            // Send enable or disable messages
            osMessageQueuePut(_task_queues->optical_sensor, &opticalEncoderEnable, 0, osWaitForever);
            osMessageQueuePut(_task_queues->force_sensor, &forceSensorEnable, 0, osWaitForever);
            

            _prevInSession = InSessionStatus;


        }

        // Things that need to occur every pass of the loop
        if (InSessionStatus)
        {
            // Get PID enabled status and enable PID Controller
            bool PIDEnabled = _fsm.GetPIDEnabledStatus();

            // Only if the status has changed
            if (PIDEnabled ^ _prevPIDEnabled)
            {
                session_controller_to_pid_controller pid_msg;
                pid_msg.enable_status = PIDEnabled;
                pid_msg.desired_rpm = _fsm.GetDesiredRpm();
                osMessageQueuePut(_task_queues->pid_controller, &pid_msg, 0, 0);
                _prevPIDEnabled = PIDEnabled;
            }
            
            // Always run since the PID controller could be turned off while in-session
            float newDutyCycle = _fsm.GetDesiredBpmDutyCycle();
            if (newDutyCycle != static_cast<float>(-1))
            {
                session_controller_to_bpm bpmSettings;
                
                bpmSettings.op = START_PWM;
                bpmSettings.new_duty_cycle_percent =  newDutyCycle;

                osMessageQueuePut(_task_queues->bpm_controller, &bpmSettings, 0, osWaitForever);
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



