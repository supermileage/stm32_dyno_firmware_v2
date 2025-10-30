#include "bpm/bpm.h"

// Need Init
// bpm_control_enable
// pass timer into constructor

#define MAX_DUTY_CYCLE 200
#define MIN_DUTY_CYCLE 0

class BPM
{
	public:
		BPM(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmqHandle);
		virtual ~BPM() = default;
		bool Init();
		void Run();

	private:
		void SetDutyCycle(uint16_t new_duty_cycle);
		void PWMToggle();


		TIM_HandleTypeDef* _timer;
		uint16_t _dutyCycle;
		bool _bpmCtrlEnable;
		osMessageQueueId_t _fromSCqHandle;
		session_controller_to_bpm _msg;
};

BPM::BPM(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmqHandle)
    : _timer(timer),
      _dutyCycle(MIN_DUTY_CYCLE),
      _bpmCtrlEnable(false),
	  _fromSCqHandle(sessionControllerToBpmqHandle),
	  _msg{}
{}



bool BPM::Init()
{
	return true;
}


void BPM::Run(void)
{
	osStatus_t status;

	while(1) {

		status = osMessageQueueGet(_fromSCqHandle, &_msg, NULL, 0);

		if (status != osOK)
			continue;

		switch(_msg.op)
		{
			case START_PWM:
				_bpmCtrlEnable = true;
				PWMToggle(); // start_pwm
				break;
			case STOP_PWM:
				_bpmCtrlEnable = false;
				PWMToggle();
				break;
			case SET_DUTY_CYCLE:
				SetDutyCycle(_msg.new_duty_cycle);
				break;
			default:
				break;
		}
	}
}


void BPM::SetDutyCycle(uint16_t new_duty_cycle)
	{
		if (new_duty_cycle < MIN_DUTY_CYCLE)
			new_duty_cycle = MIN_DUTY_CYCLE;
		else if (new_duty_cycle > MAX_DUTY_CYCLE)
			new_duty_cycle = MAX_DUTY_CYCLE;

		_dutyCycle = new_duty_cycle;

		__HAL_TIM_SET_COMPARE(_timer, TIM_CHANNEL_1, new_duty_cycle);
	}

void BPM::PWMToggle()
{
	if (_bpmCtrlEnable)
	{
		HAL_TIM_PWM_Start(_timer, TIM_CHANNEL_1);
	}
	else
	{
		HAL_TIM_PWM_Stop(_timer, TIM_CHANNEL_1);
	}
}

extern "C" void bpm_main(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmqHandle)
{
	BPM bpm = BPM(timer, sessionControllerToBpmqHandle);

	if (!bpm.Init())
	{
		return;
	}


	bpm.Run();
}
