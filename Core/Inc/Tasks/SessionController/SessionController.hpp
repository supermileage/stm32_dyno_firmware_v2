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

};



#endif /* INC_SESSIONCONTROLLER_SESSIONCONTROLLER_HPP_ */
