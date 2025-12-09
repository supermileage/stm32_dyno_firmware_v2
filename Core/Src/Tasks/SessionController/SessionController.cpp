#include <Tasks/SessionController/SessionController.hpp>

SessionController::SessionController(session_controller_os_tasks* task_queues) : 
                _forcesensor_buffer_reader(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
                _optical_encoder_buffer_reader(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
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
    forcesensor_output_data force_data;
    optical_encoder_output_data optical_encoder_data;

    memset(&force_data, 0, sizeof(force_data));
    memset(&optical_encoder_data, 0, sizeof(optical_encoder_data));

    while(1)
    {
        osDelay(SESSIONCONTROLLER_TASK_OSDELAY);
        
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
                pid_msg.desired_angular_velocity = _fsm.GetDesiredAngularVelocity();
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

            // Get the most recent force sensor data
            while(_forcesensor_buffer_reader.GetElementAndIncrementIndex(force_data));

            // Get the most recent optical encoder data
            while(_optical_encoder_buffer_reader.GetElementAndIncrementIndex(optical_encoder_data));

            // TO UPDATE
            float angularAcceleration = 0;
            float angularVelocity = optical_encoder_data.angular_velocity;
            float force = force_data.force;
            
            float torque = CalculateTorque(angularAcceleration, force, angularVelocity);
            
            _fsm.DisplayTorque(torque);
            _fsm.DisplayPower(CalculatePower(torque, angularVelocity));
            _fsm.DisplayRpm(optical_encoder_data.angular_velocity);


        }

        
            

            

    }
}

float SessionController::CalculateTorque(float angularAcceleration, float force, float angularVelocity)
{
    return (MOMENT_OF_INERTIA_KG_M2 * angularAcceleration + force * DISTANCE_FROM_FORCE_SENSOR_TO_CENTER_OF_SHAFT_M + CalculateMechanicalLosses(angularAcceleration, angularVelocity));
}

float SessionController::CalculatePower(float torque, float angularVelocity)
{
    return torque * angularVelocity;
}

float SessionController::CalculateMechanicalLosses(float angularAcceleration, float angularVelocity)
{
    return 0;
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




