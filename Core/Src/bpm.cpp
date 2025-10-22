#include "bpm.h"

// Need Init
// bpm_control_enable
// pass timer into constructor

#define MAX_DUTY_CYCLE 100
#define MIN_DUTY_CYCLE 0
#define BPM_CONTROL_STATE 1

class BPM
{
	public:
		BPM(TIM_HandleTypeDef* timer);
		virtual ~BPM() = default;
		bool Init();
		void Run();

	private:
		TIM_HandleTypeDef* _timer;
		bool _bpm_control_state;
		uint16_t _duty_cycle;
};

BPM::BPM() :
	_duty_cycle(MIN_DUTY_CYCLE)
{}



bool BPM::Init()
{
	return true;
}


void BPM::Run(void)
{

	session_controller_to_lumex_lcd* msg;
	osMessageQueueGet(_osHandle, &msg, 500);
	switch(msg->op)
	{
		case START:
			HAL_TIM_PWM_Start(_timer, TIM_CHANNEL_1); // start_pwm
			break;
		case STOP:
			HAL_TIM_PWM_Stop(_timer, TIM_CHANNEL_1)
			break;
		case NEW_DUTY_CYCLE:
			DisplayString(msg->row, msg->column, msg->display_string);
			break;
		default:
			break;
	}

}

void BPMSetDutyCycle(sessioncontroller* controller, uint8_t new_duty_cycle)
	{
		if (new_duty_cycle < MIN_DUTY_CYCLE)
			new_duty_cycle = MIN_DUTY_CYCLE;
		else if (new_duty_cycle > MAX_DUTY_CYCLE)
			new_duty_cycle = MAX_DUTY_CYCLE;

		_dutyCycle = new_duty_cycle;

		__HAL_TIM_SET_COMPARE(_timer, TIM_CHANNEL_1, new_duty_cycle);
	}

void BPMToggle(sessioncontroller* controller)
{
	if (controller->_bpm_control_state)
	{
		HAL_TIM_PWM_Start(_timer, TIM_CHANNEL_1);
	}
	else
	{
		HAL_TIM_PWM_Stop(_timer, TIM_CHANNEL_1);
	}
}

