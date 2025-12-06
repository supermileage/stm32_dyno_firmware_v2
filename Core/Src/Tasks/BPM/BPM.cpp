#include "Tasks/BPM/bpm_main.h"
#include "Tasks/BPM/BPM.hpp"

BPM::BPM(osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidToBpmHandle)
    : // this comes directly from circular_buffers.h and config.h
	_buffer_writer(bpm_circular_buffer, &bpm_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
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
	bool readFromPID = false;

	while(1) {

		while (osMessageQueueGet(_fromSCHandle, &scMsg, NULL, 0) == osOK)
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
				bpm_output_data outputData;
				outputData.timestamp = get_timestamp();
				outputData.duty_cycle = latestDutyCycle;
				_buffer_writer.WriteElementAndIncrementIndex(outputData);
			}
		}

	}
}


void BPM::TogglePWM(bool enable)
{
	// if master enables and BPM was previously disabled, then start PWM
	if (enable && !_bpmCtrlEnabled)
	{
		HAL_TIM_PWM_Start(bpmTimer, TIM_CHANNEL_1);
	}
	// if master enables and BPM was previously enabled, then stop PWM
	else if (!enable && _bpmCtrlEnabled)
	{
		HAL_TIM_PWM_Stop(bpmTimer, TIM_CHANNEL_1);
	}

	_bpmCtrlEnabled = enable;
}


void BPM::SetDutyCycle(float new_duty_cycle_percent)
{

	if (new_duty_cycle_percent < MIN_DUTY_CYCLE_PERCENT)
		new_duty_cycle_percent = MIN_DUTY_CYCLE_PERCENT;
	else if (new_duty_cycle_percent > MAX_DUTY_CYCLE_PERCENT)
		new_duty_cycle_percent = MAX_DUTY_CYCLE_PERCENT;

	uint16_t new_duty_cycle = MAX_DUTY_CYCLE_PERCENT * __HAL_TIM_GET_AUTORELOAD(bpmTimer);

	__HAL_TIM_SET_COMPARE(bpmTimer, TIM_CHANNEL_1, new_duty_cycle);
}

extern "C" void bpm_main(osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidControllerToBpmHandle)
{
	BPM bpm = BPM(sessionControllerToBpmHandle, pidControllerToBpmHandle);

	if (!bpm.Init())
	{
		return;
	}


	bpm.Run();
}
