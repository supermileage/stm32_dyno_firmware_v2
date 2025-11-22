#ifndef INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_
#define INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_

#include "main.h"
#include "cmsis_os.h"

class SessionController
{
    public:
        SessionController();
        ~SessionController() = default;

        bool Init(void);
        void Run(void);

    private:
        void HandleUserInputs();
        void HandleRotaryEncoderInput(bool positiveTick);
        void HandleRotaryEncoderSwInput();
        void HandleButtonBackInput();
        void HandleButtonSelectInput();
        void HandleButtonBrakeInput(bool isEnabled);

        uint32_t _session_controller_input_data_index;

};



#endif /* INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_ */
