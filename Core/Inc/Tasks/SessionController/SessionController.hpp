#ifndef INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_
#define INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_

#include "main.h"
#include "cmsis_os.h"

#include "FiniteStateMachine.hpp"

#include "input_manager_interrupts.h"
#include "sessioncontroller_main.h"

#ifdef __cplusplus
extern "C" {
#endif

class SessionController
{
    public:
        SessionController(session_controller_os_tasks* task_queues);
        ~SessionController() = default;

        bool Init(void);
        void Run(void);

    private:
        FSM _fsm;
        session_controller_os_tasks* _task_queues;

        bool _prevUSBLoggingEnabled;
        bool _prevSDLoggingEnabled;
        bool _prevPIDEnabled;
        bool _prevInSession;

};

#ifdef __cplusplus
}
#endif





#endif /* INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_ */
