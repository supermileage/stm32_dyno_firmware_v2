#ifndef BPM_H_
#define BPM_H_

#include "main.h"
#include "cmsis_os2.h"


#ifdef __cplusplus
extern "C" {
#endif

void bpm_main(osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidToBpmHandle);

#ifdef __cplusplus
}
#endif

#endif /* BPM_H_ */
