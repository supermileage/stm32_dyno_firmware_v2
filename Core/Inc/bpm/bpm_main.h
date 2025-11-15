#ifndef BPM_H_
#define BPM_H_

#include "main.h"
#include "cmsis_os.h"


#ifdef __cplusplus
extern "C" {
#endif

void bpm_main(TIM_HandleTypeDef* timer, osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidToBpmHandle);

#ifdef __cplusplus
}
#endif

#endif /* BPM_H_ */
