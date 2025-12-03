#ifndef INC_PID_PID_HPP_
#define INC_PID_PID_HPP_

#include "main.h"

#include "cmsis_os2.h"

#include "MessagePassing/osqueue_helpers.h"
#include "MessagePassing/messages.h"
#include "MessagePassing/circular_buffers.h"

#include "CircularBufferReader.hpp"

#include "config.h"

#include <stdint.h>

class PIDController
{
	public:
		PIDController(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState);

		virtual ~PIDController() = default;

		bool Init();
		void Run();
	private:
		CircularBufferReader<optical_encoder_output_data> _buffer_reader;

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

		void ReceiveInstruction();
		void SendDutyCycle(float new_duty_cycle_percent);

		void Reset();


};


#endif /* INC_PID_PID_HPP_ */
