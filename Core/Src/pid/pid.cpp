#include "pid/pid.h"

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState) :
			_qHandleSCRcv(sessionControllerToPidControllerHandle),
			_qHandleOERcv(opticalEncoderToPidControllerHandle),
			_qHandleBPMSend(pidToBpmHandle),
			_enabled(initialState),
			_prev_timestamp(0),
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

		uint32_t _prev_timestamp;
		uint32_t _cur_timestamp;

		float _prev_rpm;
		float _cur_rpm;

		void ReceiveInstruction();
		void ReceiveLatestOpticalEncoderData();
		void SendPWMDuty();
		void ClearSendQueue();

		void Controller();

};

bool PIDController::Init()
{
	return true;
}

void PIDController::Run()
{

}

void PIDController::ReceiveInstruction()
{
	osStatus_t status;

	bool enable_status;

	status = osMessageQueueGet(_qHandleSCRcv, &enable_status, 0, 0);

	if (status != osOK)
	{
		return;
	}

	_enabled = enable_status;
}

void PIDController::ClearSendQueue()
{
	osStatus_t status;

	while(status = osMessageQueueGet(_qHandleBPMSend, &, NULL, 0) == osOK);

	return;
}



