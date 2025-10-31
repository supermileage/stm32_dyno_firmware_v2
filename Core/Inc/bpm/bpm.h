#ifndef BPM_H_
#define BPM_H_

#include "main.h"
#include "cmsis_os.h"

#include "osQueue/osqueue_task_to_task.h"


#ifdef __cplusplus
extern "C" {
#endif

void bpm_main(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmqHandle);

#ifdef __cplusplus
}
#endif

#endif /* BPM_H_ */
