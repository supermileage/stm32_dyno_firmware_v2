#include <Tasks/USB/usbcontroller_main.h>
#include <Tasks/USB/USBController.hpp>

extern size_t optical_encoder_circular_buffer_index_writer;
extern size_t forcesensor_circular_buffer_index_writer;
extern size_t bpm_circular_buffer_index_writer;

extern optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];
extern forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];
extern bpm_output_data bpm_circular_buffer[BPM_CIRCULAR_BUFFER_SIZE];

extern size_t task_error_circular_buffer_index_writer;
extern task_error_data task_error_circular_buffer[TASK_ERROR_CIRCULAR_BUFFER_SIZE];



USBController::USBController(osMessageQueueId_t sessionControllerToUsbController, osMessageQueueId_t taskMonitorToUsbControllerHandle)
    : _task_errors_buffer_reader(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
        _buffer_reader_optical_encoder(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_forcesensor(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _taskMonitorToUsbControllerHandle(taskMonitorToUsbControllerHandle),
      _sessionControllerToUsbController(sessionControllerToUsbController),
      _txBuffer{},
      _txBufferIndex(0)
{}

bool USBController::Init()
{
	return true;
}

void USBController::Run()
{
    bool enableUSB = false;

    while (1)
    {
        // GetLatestFromQueue(
        //     _sessionControllerToUsbController,
        //     &enableUSB,
        //     sizeof(enableUSB),
        //     enableUSB ? 0 : osWaitForever
        // );

        if (!enableUSB)
        {
            continue;
        }

        // Process optical encoder data
        ProcessTaskData(_buffer_reader_optical_encoder, TASK_ID_OPTICAL_ENCODER);

        // Process force sensor data
        ProcessTaskData(_buffer_reader_forcesensor, TASK_ID_FORCE_SENSOR);

        // Process BPM data
        ProcessTaskData(_buffer_reader_bpm, TASK_ID_BPM_CONTROLLER);

        #if TASK_MONITOR_TASK_ENABLE
        // Process Task Monitor data
        ProcessTaskData(_taskMonitorToUsbControllerHandle, TASK_ID_TASK_MONITOR);
        #endif

        ProcessErrorsAndWarnings();

        

        if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
            continue;
        }
        _txBufferIndex = 0;

        osDelay(USB_TASK_OSDELAY);
    }
}



// void USBController::Run()
// {
// 	uint8_t msg[6];

// 	for (;;)
// 	{
// 	    msg[0] = 'B';
// 	    msg[1] = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port,  BTN_BRAKE_Pin)  ? '1' : '0';
// 	    msg[2] = 'S';
// 	    msg[3] = HAL_GPIO_ReadPin(BTN_SELECT_GPIO_Port, BTN_SELECT_Pin) ? '1' : '0';
// 	    msg[4] = 'K';
// 	    msg[5] = '\n';

// 	    while (CDC_Transmit_FS(msg, sizeof(msg)) == USBD_BUSY)
// 	    {
// 	        osDelay(1);
// 	    }

// 	    osDelay(100);
// 	}

// }

void USBController::ProcessErrorsAndWarnings()
{
    while(_task_errors_buffer_reader.HasData()) {
        // Ensure the buffer is not full before adding data
        if (BufferFull(sizeof(task_error_data))) {
            while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
                osDelay(1);
            }
            _txBufferIndex = 0;
        }

        task_error_data error_data;
        if (_task_errors_buffer_reader.GetElementAndIncrementIndex(error_data)) {
            usb_msg_header_t header = 
            {
                .msg_type = (error_data.error_id >= WARNING_ENUM_OFFSET) ? USB_MSG_WARNING : USB_MSG_ERROR,
                .module_id = TASK_ID_NO_TASK,
                .payload_len = sizeof(task_error_data)
            };

            AddToBuffer(&header, sizeof(header));
            AddToBuffer(&error_data, sizeof(task_error_data));
        }
    }
}

void USBController::AddToBuffer(void* msg, size_t msgSize) {
	memcpy(_txBuffer + _txBufferIndex, msg, msgSize); 
	_txBufferIndex += msgSize;
}


bool USBController::BufferFull(std::size_t msgSize)
{   
    if (_txBufferIndex + sizeof(usb_msg_header_t) + msgSize >= USB_TX_BUFFER_SIZE) {
        return true;   
    }
	return false;

}

extern "C" void usbcontroller_main(osMessageQueueId_t sessionControllerToUsbController, osMessageQueueId_t taskMonitorToUsbControllerHandle)
{
	USBController usb = USBController(sessionControllerToUsbController, taskMonitorToUsbControllerHandle);

	if (!usb.Init())
	{
		 osThreadSuspend(osThreadGetId());;
	}

	usb.Run();
}
