#ifndef INC_TASKS_SENSORBOARDCONTROLLER_SENSORBOARDCONTROLLER_MAIN_H_
#define INC_TASKS_SENSORBOARDCONTROLLER_SENSORBOARDCONTROLLER_MAIN_H_

#include "main.h"
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

void sensorboardcontroller_main(osMessageQueueId_t sessionControllerToSensorBoardControllerHandle, osMutexId_t usart1Mutex);

#ifdef __cplusplus
}
#endif

#endif /* INC_TASKS_SENSORBOARDCONTROLLER_SENSORBOARDCONTROLLER_MAIN_H_ */