#include "Tasks/TaskMonitor/TaskMonitor.hpp"


TaskMonitor::TaskMonitor(taskmonitor_osthreadids* osthreadid_ptrs, osMessageQueueId_t taskMonitorToUsbControllerHandle) :
    _task_error_buffer_writer(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
	_osThreadIdPtrs(osthreadid_ptrs), 
	_taskMonitorToUsbControllerHandle(taskMonitorToUsbControllerHandle)
{}


bool TaskMonitor::Init()
{
    if (_osThreadIdPtrs == nullptr
		#if SESSION_CONTROLLER_TASK_ENABLE
		|| _osThreadIdPtrs->session_controller == nullptr
		#endif
		#if USB_CONTROLLER_TASK_ENABLE
		|| _osThreadIdPtrs->usb_controller == nullptr
		#endif
		#if SD_CONTROLLER_TASK_ENABLE
		|| _osThreadIdPtrs->sd_controller == nullptr
		#endif
		#if OPTICAL_ENCODER_TASK_ENABLE
		|| _osThreadIdPtrs->optical_sensor == nullptr
		#endif
		#if FORCE_SENSOR_ADS1115_TASK_ENABLE || FORCE_SENSOR_ADC_TASK_ENABLE
		|| _osThreadIdPtrs->force_sensor == nullptr
		#endif
		#if BPM_CONTROLLER_TASK_ENABLE
		|| _osThreadIdPtrs->bpm_controller == nullptr
		#endif
		#if PID_CONTROLLER_TASK_ENABLE
		|| _osThreadIdPtrs->pid_controller == nullptr
		#endif
		#if LUMEX_LCD_TASK_ENABLE
		|| _osThreadIdPtrs->lumex_lcd == nullptr
		#endif
	)
	{
		_task_error_buffer_writer.WriteElementAndIncrementIndex(ERROR_TASK_MONITOR_INVALID_THREAD_ID_POINTER);
		return false;
	}
	
	return true;
}

void TaskMonitor::GetTaskDataAndSendToUsbController(task_monitor_task_opcode op, osThreadId_t thread_id)
{
	task_monitor_to_usb_controller msg{};
	
	msg.timestamp = get_timestamp();
	msg.op = op;
	msg.task_state = osThreadGetState(thread_id);
	UBaseType_t free_words = uxTaskGetStackHighWaterMark((TaskHandle_t)thread_id);
	msg.free_bytes = free_words * sizeof(StackType_t);

	osMessageQueuePut(_taskMonitorToUsbControllerHandle, &msg, 0, osWaitForever);
}


void TaskMonitor::Run()
{
    while(1)
	{
		#if SESSION_CONTROLLER_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_SESSION_CONTROLLER_TASK, _osThreadIdPtrs->session_controller);
		#endif
		#if USB_CONTROLLER_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_USB_CONTROLLER_TASK, _osThreadIdPtrs->usb_controller);
		#endif
		#if SD_CONTROLLER_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_SD_CONTROLLER_TASK, _osThreadIdPtrs->sd_controller);
		#endif
		#if OPTICAL_ENCODER_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_OPTICAL_ENCODER_TASK, _osThreadIdPtrs->optical_sensor);
		#endif 
		#if FORCE_SENSOR_ADS1115_TASK_ENABLE || FORCE_SENSOR_ADC_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_FORCE_SENSOR_TASK, _osThreadIdPtrs->force_sensor);
		#endif 
		#if BPM_CONTROLLER_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_BPM_CONTROLLER_TASK, _osThreadIdPtrs->bpm_controller);
		#endif
		#if PID_CONTROLLER_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_PID_CONTROLLER_TASK, _osThreadIdPtrs->pid_controller);
		#endif
		#if LUMEX_LCD_TASK_ENABLE
		GetTaskDataAndSendToUsbController(TASK_MONITOR_LUMEX_LCD_TASK, _osThreadIdPtrs->lumex_lcd);
		#endif

		GetTaskDataAndSendToUsbController(TASK_MONITOR_TASK_MONITOR_TASK, osThreadGetId());
		
		osDelay(TASK_MONITOR_TASK_OSDELAY);
	}

}

#if defined(configCHECK_FOR_STACK_OVERFLOW) && (configCHECK_FOR_STACK_OVERFLOW > 0)
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Optional: halt the system
    taskDISABLE_INTERRUPTS();
    for(;;);
}
#endif

extern void taskmonitor_main(taskmonitor_osthreadids* osthreadid_ptrs, osMessageQueueId_t taskMonitorToUsbControllerHandle)
{
	TaskMonitor monitor = TaskMonitor(osthreadid_ptrs, taskMonitorToUsbControllerHandle);

	if (!monitor.Init())
	{
		osDelay(osWaitForever);
	}

	monitor.Run();
}