#include <Tasks/PID/pid_main.h>
#include <Tasks/PID/PID.hpp>

PIDController::PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState) : 
			_buffer_reader(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),			
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
        // Process any incoming instructions
        ReceiveInstruction();

        if (!_enabled)
        {
            // If PID is disabled, clear any pending BPM commands
            EmptyQueue(_pidToBpmHandle, sizeof(session_controller_to_bpm));
            continue; // skip PID processing
        }

		// Try to get the first new element
		if (!_buffer_reader.GetElementAndIncrementIndex(latestOpticalEncoderData)) {
			// Nothing new, go back to top of loop
			continue;
		}

		// There is at least one element, keep fetching until the last one
		while (_buffer_reader.GetElementAndIncrementIndex(latestOpticalEncoderData));

        // Update current values
        _curTimestamp = latestOpticalEncoderData.timestamp;
        _curAngularVelocity = latestOpticalEncoderData.angular_velocity;

        // Compute time delta safely, handling timer overflow
        timeDelta = GetTimeDelta();

        // PID error calculation
        _error = static_cast<float>(_desiredAngularVelocity) - _curAngularVelocity;

        derivative = (_error - _prevError) / (float)timeDelta;
        integral += _error * (float)timeDelta;

        // PID control output
        controlValue = K_P * _error
                     + K_D * derivative
                     + K_I * integral;

        // Update previous values for next iteration
        _prevTimestamp = _curTimestamp;
        _prevError = _error;

        // Send controlValue to BPM queue
        SendDutyCycle(controlValue);

        // Optional: small delay to yield (not strictly necessary since blocking)
        osDelay(1);
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

void PIDController::ReceiveInstruction()
{
	osStatus_t status;

	session_controller_to_pid_controller msg;

	status = osMessageQueueGet(_sessionControllerToPidHandle, &msg, NULL, 0);

	if (status != osOK)
	{
		return;
	}

	_enabled = msg.enable_status;
	_desiredAngularVelocity = msg.desired_angular_velocity;

	if (_enabled)
	{
		Reset();
	}
}

void PIDController::SendDutyCycle(float new_duty_cycle_percent)
{

	if (osMessageQueuePut(_pidToBpmHandle, &new_duty_cycle_percent, 0, 0) != osOK)
	{
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






