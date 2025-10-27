#include "pid/pid.h"

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState, TIM_HandleTypeDef* bpmTimer) :
			_qHandleSCRcv(sessionControllerToPidControllerHandle),
			_qHandleOERcv(opticalEncoderToPidControllerHandle),
			_qHandleBPMSend(pidToBpmHandle),
			_enabled(initialState),
			_curTimestamp(0),
			_curRpm(static_cast<float>(0)),
			_desiredRpm(static_cast<float>(0)),
			_prevError(static_cast<float>(0)),
			_error(static_cast<float>(0)),
			_bpmTimerAutoreload(__HAL_TIM_GET_AUTORELOAD(bpmTimer)),
			_oeMsg{}

		{

		}

		virtual ~PIDController() = default;

		bool Init();
		void Run();
	private:
		osMessageQueueId_t _qHandleSCRcv;
		osMessageQueueId_t _qHandleOERcv;
		osMessageQueueId_t _qHandleBPMSend;
		bool _enabled;

		uint32_t _curTimestamp;

		float _curRpm;

		float _desiredRpm;

		float _prevError;
		float _error;

		uint16_t _bpmTimerAutoreload;

		optical_encoder_to_pid_controller _oeMsg;

		void ReceiveInstruction();
		void ReceiveLatestOpticalEncoderData();
		void SendPWMDuty();

		void Reset();


};

bool PIDController::Init()
{
	Reset();
	return true;
}

void PIDController::Run()
{
	float value = 0;
	while(true)
	{
		ReceiveInstruction();

		if (!_enabled)
		{
			EmptyQueue(_qHandleBPMSend);
		}
		else
		{
			ReceiveLatestOpticalEncoderData()

			_error = (_desiredRpm - _curRpm);
			value = K_P * _error + K_D * _


			_prevError = _error;
		}
	}


}

void PIDController::Reset()
{
	_curTimestamp = 0;
	_curRpm = static_cast<float>(0);
	_prevTimestamp = 0;
	_prevRpm = static_cast<float>(0);

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

void PIDController::ReceiveLatestOpticalEncoderData()
{
	osStatus_t status;

	while ((status = osMessageQueueGet(_qHandleOERcv, &_oeMsg, NULL, 0)) == osOK)
	{
		_curTimestamp = _oeMsg.timestamp;
		_curRpm = _oeMsg.rpm;
	}

}

void PIDController::SendPWMDuty(uint16_t new_duty_cycle)
{

	if (osMessageQueuePut(_qHandleBPMSend, &new_duty_cycle, 0, osWaitForever) != osOK)
	{
		return;
	}

}






