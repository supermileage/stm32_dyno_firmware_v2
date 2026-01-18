#ifndef INC_TASKS_PID_PID_HPP_
#define INC_TASKS_PID_PID_HPP_

#include "main.h"

#include "cmsis_os2.h"

#include "MessagePassing/osqueue_helpers.h"
#include "MessagePassing/msgq_messages.h"
#include "MessagePassing/circular_buffers.h"
#include "MessagePassing/errors.h"

#include "CircularBufferReader.hpp"
#include "CircularBufferWriter.hpp"

#include "TimeKeeping/timestamps.h"

#include "Config/config.h"

#include <stdint.h>

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t pidControllerToSessionControllerAckHandle, osMessageQueueId_t pidToBpmHandle, osMutexId_t throttleControlMutex, bool initialState);

		~PIDController() = default;

		bool Init();
		void Run();
	private:
		CircularBufferReader<optical_encoder_output_data> _data_buffer_reader;
		CircularBufferWriter<task_error_data> _task_error_buffer_writer;

		osMessageQueueId_t _sessionControllerToPidHandle;
		osMessageQueueId_t _pidControllerToSessionControllerAckHandle;
		osMessageQueueId_t _pidToBpmHandle;
		osMutexId_t _usart1Mutex;
		bool _enabled;

		uint32_t _curTimestamp;
		uint32_t _prevTimestamp;

		float _curAngularVelocity;

		float _desiredAngularVelocity;

		float _prevError;
		float _error;

		float GetTimeDelta();

		void SendBrakeDutyCycle(float new_duty_cycle_percent);
		void SendThrottleDutyCycle(float new_duty_cycle_percent);

		void Reset();


};


#endif /* INC_TASKS_PID_PID_HPP_ */
