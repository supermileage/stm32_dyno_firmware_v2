#include "pid/pid.h"

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState);

		virtual ~PIDController() = default;

		bool Init();
		void Run();
	private:
		osMessageQueueId_t _qHandleSCRcv;
		osMessageQueueId_t _qHandleOERcv;
		osMessageQueueId_t _qHandleBPMSend;
		bool _enabled;

		uint32_t _curTimestamp;
		uint32_t _prevTimestamp;

		float _curRpm;

		float _desiredRpm;

		float _prevError;
		float _error;

		float GetTimeDelta();

		void ReceiveInstruction();
		void SendDutyCycle(float new_duty_cycle_percent);

		void Reset();


};

PIDController::PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState) : 
			_qHandleSCRcv(sessionControllerToPidControllerHandle),
			_qHandleOERcv(opticalEncoderToPidControllerHandle),
			_qHandleBPMSend(pidToBpmHandle),
			_enabled(initialState),
			_curTimestamp(0),
			_prevTimestamp(0),
			_curRpm(static_cast<float>(0)),
			_desiredRpm(static_cast<float>(0)),
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
    optical_encoder_to_pid_controller latestOE;

    while (true)
    {
        // Process any incoming instructions
        ReceiveInstruction();

        if (!_enabled)
        {
            // If PID is disabled, clear any pending BPM commands
            EmptyQueue(_qHandleBPMSend, sizeof(session_controller_to_bpm));
            continue; // skip PID processing
        }

        // BLOCKING: wait forever for the latest optical encoder message
        GetLatestFromQueue(_qHandleOERcv, &latestOE, sizeof(latestOE), osWaitForever);

        // Update current values
        _curTimestamp = latestOE.timestamp;
        _curRpm = latestOE.rpm;

        // Compute time delta safely, handling timer overflow
        timeDelta = GetTimeDelta();

        // PID error calculation
        _error = _desiredRpm - _curRpm;

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
}

void PIDController::Reset()
{
	_curTimestamp = 0;
	_prevTimestamp = 0;
	_curRpm = static_cast<float>(0);

	_error = static_cast<float>(0);
	_prevError = static_cast<float>(0);
}

void PIDController::ReceiveInstruction()
{
	osStatus_t status;

	session_controller_to_pid_controller msg;

	status = osMessageQueueGet(_qHandleSCRcv, &msg, NULL, 0);

	if (status != osOK)
	{
		return;
	}

	_enabled = msg.enable_status;
	_desiredRpm = msg.desired_rpm;

	if (_enabled)
	{
		Reset();
	}
}

void PIDController::SendDutyCycle(float new_duty_cycle_percent)
{

	if (osMessageQueuePut(_qHandleBPMSend, &new_duty_cycle_percent, 0, 0) != osOK)
	{
		return;
	}

}

extern "C" void pid_main(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState) 
{
	PIDController controller = PIDController(sessionControllerToPidControllerHandle, opticalEncoderToPidControllerHandle, pidToBpmHandle, initialState);

	if (!controller.Init())
	{
		return;
	}


	controller.Run();
}






