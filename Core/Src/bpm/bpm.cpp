#include "bpm/bpm.h"

class BPM
{
	public:
		BPM(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidToBpmHandle);
		virtual ~BPM() = default;
		bool Init();
		void Run();

	private:
		void SetDutyCycle(float new_duty_cycle_percent);
		void TogglePWM(bool enable);


		TIM_HandleTypeDef* _timer;
		bool _bpmCtrlEnabled;

		osMessageQueueId_t _fromSCHandle;
		osMessageQueueId_t _fromPIDHandle;

};

BPM::BPM(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidToBpmHandle)
    : _timer(timer),
	  _bpmCtrlEnabled(false),
	  _fromSCHandle(sessionControllerToBpmHandle),
	  _fromPIDHandle(pidToBpmHandle)
{}


bool BPM::Init()
{
	return true;
}


void BPM::Run(void)
{
	session_controller_to_bpm scMsg;
	bool readFromPID = false;;

	while(1) {

		if (osMessageQueueGet(_fromSCHandle, &scMsg, NULL, 0) == osOK)
		{
			switch(scMsg.op)
			{
				case READ_FROM_PID:
					readFromPID = true;
					break;
				case START_PWM:
					SetDutyCycle(scMsg.new_duty_cycle_percent);
					TogglePWM(true);
					readFromPID = false;
					break;
				case STOP_PWM:
					TogglePWM(false);
					readFromPID = false;
					break;
				default:
					readFromPID = false;
					break;
			}


		}
		if (readFromPID)
		{
			float latestDutyCycle;
			// Get the latest available value (non-blocking)
			if (GetLatestFromQueue(_fromPIDHandle, &latestDutyCycle, sizeof(latestDutyCycle), 0))
			{
				SetDutyCycle(latestDutyCycle);
				TogglePWM(true);
			}
		}

	}
}


void BPM::TogglePWM(bool enable)
{
	// if master enables and BPM was previously disabled, then start PWM
	if (enable && !_bpmCtrlEnabled)
	{
		HAL_TIM_PWM_Start(_timer, TIM_CHANNEL_1);
	}
	// if master enables and BPM was previously enabled, then stop PWM
	else if (!enable && _bpmCtrlEnabled)
	{
		HAL_TIM_PWM_Stop(_timer, TIM_CHANNEL_1);
	}

	_bpmCtrlEnabled = enable;
}


void BPM::SetDutyCycle(float new_duty_cycle_percent)
{

	if (new_duty_cycle_percent < MIN_DUTY_CYCLE_PERCENT)
		new_duty_cycle_percent = MIN_DUTY_CYCLE_PERCENT;
	else if (new_duty_cycle_percent > MAX_DUTY_CYCLE_PERCENT)
		new_duty_cycle_percent = MAX_DUTY_CYCLE_PERCENT;

	uint16_t new_duty_cycle = MAX_DUTY_CYCLE_PERCENT * __HAL_TIM_GET_AUTORELOAD(_timer);

	__HAL_TIM_SET_COMPARE(_timer, TIM_CHANNEL_1, new_duty_cycle);
}


extern "C" void bpm_main(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidControllerToBpmHandle)
{
	BPM bpm = BPM(timer, sessionControllerToBpmHandle, pidControllerToBpmHandle);

	if (!bpm.Init())
	{
		return;
	}


	bpm.Run();
}
