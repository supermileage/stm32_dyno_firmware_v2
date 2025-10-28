#include "osQueue/osqueue_task_to_task.h"

// Need Init
// bpm_control_enable
// pass timer into constructor

#define MAX_DUTY_CYCLE 200
#define MIN_DUTY_CYCLE 0

class BPM
{
	public:
		BPM(TIM_HandleTypeDef* timer);
		virtual ~BPM() = default;
		bool Init();
		void Run();

	private:
		TIM_HandleTypeDef* _timer;
		bool _bpm_ctrl_enable;
		uint16_t _duty_cycle;
};

BPM::BPM(TIM_HandleTypeDef* timer)
    : _timer(timer),
      _duty_cycle(MIN_DUTY_CYCLE),
      _bpm_ctrl_enable(false)
{}



bool BPM::Init()
{
	return true;
}


void BPM::Run(void)
{
	while(1) {
		session_controller_to_lumex_lcd msg;
			osMessageQueueGet(_osHandle, &msg, 500);
			switch(msg->op)
			{
				case START:
					PWMToggle(TIM_CHANNEL_1); // start_pwm
					break;
				case STOP:
					PWMToggle(TIM_CHANNEL_1);
					break;
				case SET_DUTY_CYCLE:
					break;
				default:
					break;
			}
	}
}

void BPMSetDutyCycle(uint8_t new_duty_cycle)
	{
		if (new_duty_cycle < MIN_DUTY_CYCLE)
			new_duty_cycle = MIN_DUTY_CYCLE;
		else if (new_duty_cycle > MAX_DUTY_CYCLE)
			new_duty_cycle = MAX_DUTY_CYCLE;

		_dutyCycle = new_duty_cycle;

		__HAL_TIM_SET_COMPARE(_timer, TIM_CHANNEL_1, new_duty_cycle);
	}

void PWMToggle(sessioncontroller* controller)
{
	if (controller->_bpm_ctrl_enable)
	{
		HAL_TIM_PWM_Start(_timer, TIM_CHANNEL_1);
	}
	else
	{
		HAL_TIM_PWM_Stop(_timer, TIM_CHANNEL_1);
		bpm_main();
	}
}

extern "C" void bpm_main(TIM_HandleTypeDef* timer, osMessageQueueId_t bpmToSessionControllerqHandle, osMessageQueueId_t timInterruptCallbackqHandle)
{
	BPM bpm = BPM(timer);

	if (!bpm.Init())
	{
		return;
	}


	bpm.Run();
}
