#include "osQueue/osqueue_task_to_task.h"


void EmptyQueue(osMessageQueueId_t qHandle)
{
    osStatus_t status;
    uint32_t msg_size = osMessageQueueGetMsgSize(qHandle);
    uint8_t *temp = malloc(msg_size);
    if (!temp) return;

    while ((status = osMessageQueueGet(qHandle, temp, NULL, 0)) == osOK);
    free(temp);
}
