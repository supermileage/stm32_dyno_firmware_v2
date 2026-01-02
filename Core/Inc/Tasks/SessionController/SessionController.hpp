#ifndef INC_TASKS_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_
#define INC_TASKS_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_

#include <cstring>

#include "main.h"
#include "cmsis_os.h"

#include "FiniteStateMachine.hpp"

#include "MessagePassing/circular_buffers.h"
#include "MessagePassing/errors.h"

#include "CircularBufferReader.hpp"
#include "CircularBufferWriter.hpp"

#include "TimeKeeping/timestamps.h"

#include "input_manager_interrupts.h"
#include "sessioncontroller_main.h"


#ifdef __cplusplus
extern "C" {
#endif

class SessionController
{
    public:
        SessionController(session_controller_os_task_queues* task_queues);
        ~SessionController() = default;

        bool Init(void);
        void Run(void);

    private:
        CircularBufferWriter<task_errors> _task_error_buffer_writer;
        CircularBufferReader<forcesensor_output_data> _forcesensor_buffer_reader;
        CircularBufferReader<optical_encoder_output_data> _optical_encoder_buffer_reader;

        FSM _fsm;

        session_controller_os_task_queues* _task_queues;

        bool _prevUSBLoggingEnabled;
        bool _prevSDLoggingEnabled;
        bool _prevPIDEnabled;
        bool _prevInSession;

        bool CheckTaskQueuesValid();

        inline float CalculateTorque(float angularAcceleration, float force, float angularVelocity);
        inline float CalculatePower(float torque, float angularVelocity);
        inline float CalculateMechanicalLosses(float angularAcceleration, float angularVelocity);

};

#ifdef __cplusplus
}
#endif





#endif /* INC_TASKS_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_ */
