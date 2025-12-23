#include <Tasks/PID/pid_main.h>
#include <Tasks/PID/PID.hpp>

PIDController::PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState) : 
			_data_buffer_reader(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),			
            _task_error_buffer_writer(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
            _sessionControllerToPidHandle(sessionControllerToPidControllerHandle),
			_pidToBpmHandle(pidToBpmHandle),
			_enabled(initialState),
			_curTimestamp(0),
			_prevTimestamp(0),
			_curAngularVelocity(static_cast<float>(0)),
			_desiredAngularVelocity(static_cast<float>(0)),
			_prevError(static_cast<float>(0)),
			_error(static_cast<float>(0))
{}

bool PIDController::Init()
{
	Reset();
	return true;
}

void PIDController::Run()
{
    float controlValue = 0;
    float integral = 0;
    float derivative = 0;
    uint32_t timeDelta;
    optical_encoder_output_data latestOpticalEncoderData;

    while (true)
    {
        // --- Conditional blocking on instructions ---
        session_controller_to_pid_controller msg;
        bool gotInstruction = GetLatestFromQueue(
            _sessionControllerToPidHandle,
            &msg,
            sizeof(msg),
            _enabled ? 0 : osWaitForever  // poll if enabled, block if disabled
        );

        // If we got a message, update PID state
        if (gotInstruction)
        {
            _enabled = msg.enable_status;
            _desiredAngularVelocity = msg.desired_angular_velocity;

            if (_enabled)
            {
                Reset();
            }
        }

        // Skip processing if PID is disabled
        if (!_enabled)
        {
            // Clear any pending BPM commands
            EmptyQueue(_pidToBpmHandle, sizeof(session_controller_to_bpm));
            continue;
        }

        // --- Read latest optical encoder data ---
        if (!_data_buffer_reader.GetElementAndIncrementIndex(latestOpticalEncoderData))
        {
            // Nothing new, skip loop
            continue;
        }

        // Drain any remaining elements to get the latest
        while (_data_buffer_reader.GetElementAndIncrementIndex(latestOpticalEncoderData));

        // --- Update current values ---
        _curTimestamp = latestOpticalEncoderData.timestamp;
        _curAngularVelocity = latestOpticalEncoderData.angular_velocity;

        // Compute time delta safely, handling timer overflow
        timeDelta = GetTimeDelta();

        // PID error calculation
        _error = static_cast<float>(_desiredAngularVelocity) - _curAngularVelocity;
        derivative = (_error - _prevError) / static_cast<float>(timeDelta);
        integral += _error * static_cast<float>(timeDelta);

        // PID control output
        controlValue = K_P * _error
                     + K_D * derivative
                     + K_I * integral;

        // Update previous values
        _prevTimestamp = _curTimestamp;
        _prevError = _error;

        // Send control value to BPM
        SendDutyCycle(controlValue);

        // Yield to other tasks
        osDelay(PID_TASK_OSDELAY);
    }
}





float PIDController::GetTimeDelta()
{
	float timeDelta;
	
	if (_curTimestamp > _prevTimestamp)
	{
		timeDelta = _curTimestamp - _prevTimestamp;
	}
	else if (_curTimestamp == _prevTimestamp)
	{
		timeDelta = 1;
	}
	else
	{
		timeDelta = (UINT32_MAX - _prevTimestamp) + _curTimestamp + 1;
	}

	return timeDelta;
}

void PIDController::Reset()
{
	_curTimestamp = 0;
	_prevTimestamp = 0;
	_curAngularVelocity = static_cast<float>(0);

	_error = static_cast<float>(0);
	_prevError = static_cast<float>(0);
}


void PIDController::SendDutyCycle(float new_duty_cycle_percent)
{

	if (osMessageQueuePut(_pidToBpmHandle, &new_duty_cycle_percent, 0, 0) != osOK)
	{
		_task_error_buffer_writer.WriteElementAndIncrementIndex(WARNING_PID_CONTROLLER_MESSAGE_QUEUE_FULL);
        osDelay(TASK_WARNING_RETRY_OSDELAY);
        return;
	}

}

extern "C" void pid_main(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState) 
{
	PIDController controller = PIDController(sessionControllerToPidControllerHandle, pidToBpmHandle, initialState);

	if (!controller.Init())
	{
		return;
	}


	controller.Run();
}






