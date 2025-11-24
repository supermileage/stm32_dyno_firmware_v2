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
        SessionController(osMessageQueueId_t sessionControllerToLumexLcdHandle);
        ~SessionController() = default;

        bool Init(void);
        void Run(void);

    private:
        FSM _fsm;
        osMessageQueueId_t _sessionControllerToLumexLcdHandle;

        bool _prevUSBLoggingEnabled;
        bool _prevSDLoggingEnabled;
        bool _prevPIDEnabled;

};

#ifdef __cplusplus
}
#endif





#endif /* INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_ */
