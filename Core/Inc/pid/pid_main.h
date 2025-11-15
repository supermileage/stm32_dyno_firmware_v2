#ifndef INC_PID_PID_MAIN_H_
#define INC_PID_PID_MAIN_H_

#include "main.h"

#include "cmsis_os.h"


#ifdef __cplusplus
extern "C" {
#endif

void pid_main(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState);

#ifdef __cplusplus
}
#endif

#endif /* INC_PID_PID_MAIN_H_ */
