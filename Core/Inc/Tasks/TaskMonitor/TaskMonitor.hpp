#ifndef INC_TASKS_TASKMONITOR_TASKMONITOR_HPP_
#define INC_TASKS_TASKMONITOR_TASKMONITOR_HPP_

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

#include "Config/config.h"
#include "Config/debug.h" 

#include "MessagePassing/messages.h"
#include "MessagePassing/circular_buffers.h"
#include "MessagePassing/errors.h"

#include "TimeKeeping/timestamps.h"

#include "CircularBufferWriter.hpp"

#include "taskmonitor_main.h"

class TaskMonitor
{
public:
    TaskMonitor(taskmonitor_osthreadids* osthreadid_ptrs, osMessageQueueId_t taskMonitorToUsbControllerHandle);

    ~TaskMonitor() = default;

    bool Init();

    void Run();
private:
    void GetTaskDataAndSendToUsbController(task_monitor_task_opcode op, osThreadId_t thread_id);

    CircularBufferWriter<task_errors> _task_error_buffer_writer;

    taskmonitor_osthreadids* _osThreadIdPtrs;
    osMessageQueueId_t _taskMonitorToUsbControllerHandle;
};



#endif /* INC_TASKS_TASKMONITOR_TASKMONITOR_HPP_ */