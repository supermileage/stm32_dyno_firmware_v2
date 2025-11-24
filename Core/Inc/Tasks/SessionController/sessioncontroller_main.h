#ifndef INC_SESSION_CONTROLLER_SESSION_CONTROLLER_H_
#define INC_SESSION_CONTROLLER_SESSION_CONTROLLER_H_

#include "main.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

void sessioncontroller_main(osMessageQueueId_t sessionControllerToLumexLcdHandle);

#ifdef __cplusplus
}
#endif

#endif /* INC_SESSION_CONTROLLER_SESSION_CONTROLLER_H_ */
