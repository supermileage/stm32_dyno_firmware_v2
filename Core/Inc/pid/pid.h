#ifndef INC_PID_PID_H_
#define INC_PID_PID_H_

#include "main.h"

#include "cmsis_os.h"

#include "osQueue/osqueue_task_to_task.h"

#include "config.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void pid_main(osMessageQueueId_t sessionControllerToPidControllerHandle, osMessageQueueId_t opticalEncoderToPidControllerHandle, osMessageQueueId_t pidToBpmHandle, bool initialState);

#ifdef __cplusplus
}
#endif

#endif /* INC_PID_PID_H_ */
