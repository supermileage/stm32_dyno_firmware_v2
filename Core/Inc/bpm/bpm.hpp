#ifndef INC_BPM_BPM_HPP_
#define INC_BPM_BPM_HPP_

#include "main.h"
#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"

#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif


#endif /* INC_BPM_BPM_HPP_ */
