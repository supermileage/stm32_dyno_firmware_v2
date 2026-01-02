#ifndef INC_TASKS_PID_PID_HPP_
#define INC_TASKS_PID_PID_HPP_

#include "main.h"

#include "cmsis_os2.h"

#include "MessagePassing/osqueue_helpers.h"
#include "MessagePassing/messages.h"
#include "MessagePassing/circular_buffers.h"
#include "MessagePassing/errors.h"

#include "CircularBufferReader.hpp"
#include "CircularBufferWriter.hpp"

#include "Config/config.h"

#include <stdint.h>

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState);

		~PIDController() = default;

		bool Init();
		void Run();
	private:
		CircularBufferReader<optical_encoder_output_data> _data_buffer_reader;
		CircularBufferWriter<task_errors> _task_error_buffer_writer;

		osMessageQueueId_t _sessionControllerToPidHandle;
		osMessageQueueId_t _pidToBpmHandle;
		bool _enabled;

		uint32_t _curTimestamp;
		uint32_t _prevTimestamp;

		float _curAngularVelocity;

		float _desiredAngularVelocity;

		float _prevError;
		float _error;

		float GetTimeDelta();

		void SendDutyCycle(float new_duty_cycle_percent);

		void Reset();


};


#endif /* INC_TASKS_PID_PID_HPP_ */
