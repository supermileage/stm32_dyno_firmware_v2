#ifndef INC_PID_PID_HPP_
#define INC_PID_PID_HPP_

#include "main.h"

#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"

#include "config.h"

#include <stdint.h>

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState);

		virtual ~PIDController() = default;

		bool Init();
		void Run();
	private:
		osMessageQueueId_t _pidToSessionControllerHandle;
		osMessageQueueId_t _opticalEncoderToPidControllerHandle;
		osMessageQueueId_t _pidToBpmHandle;
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


#endif /* INC_PID_PID_HPP_ */
