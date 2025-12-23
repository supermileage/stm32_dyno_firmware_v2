#include <Tasks/USB/usbcontroller_main.h>
#include <Tasks/USB/USBController.hpp>

USBController::USBController(osMessageQueueId_t sessionControllerToUsbController)
    : _task_errors_buffer_reader(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
        _buffer_reader_optical_encoder(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_forcesensor(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _maxMsgSize([]() -> size_t {
          size_t opticalEncoderSize = sizeof(USBController::USBMessageIdentifier) + sizeof(optical_encoder_output_data);
          size_t forceSensorSize = sizeof(USBController::USBMessageIdentifier) + sizeof(forcesensor_output_data);
          size_t bpmSize = sizeof(USBController::USBMessageIdentifier) + sizeof(bpm_output_data);
          return std::max({opticalEncoderSize, forceSensorSize, bpmSize});
      }()),
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
        GetLatestFromQueue(
            _sessionControllerToUsbController,
            &enableUSB,
            sizeof(enableUSB),
            enableUSB ? 0 : osWaitForever
        );

        if (!enableUSB)
        {
            continue;
        }

        BlockAndSendIfBufferFull();

        // Process optical encoder data
        ProcessTaskData(_buffer_reader_optical_encoder, USBController::USBMessageIdentifier::TASK_OPTICAL_ENCODER);

        // Process force sensor data
        ProcessTaskData(_buffer_reader_forcesensor, USBController::USBMessageIdentifier::TASK_FORCESENSOR);

        // Process BPM data
        ProcessTaskData(_buffer_reader_bpm, USBController::USBMessageIdentifier::TASK_BPM);

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
        if (!BlockAndSendIfBufferFull()) {
            break;
        }

        task_errors error;
        if (_task_errors_buffer_reader.GetElementAndIncrementIndex(error)) {
            uint8_t op = static_cast<uint8_t>(
                (error >= FIRST_WARNING_NUMBER) ? USBController::USBMessageIdentifier::TASK_WARNING
                                                : USBController::USBMessageIdentifier::TASK_ERROR
            );
            AddToBuffer(&op, sizeof(USBController::USBMessageIdentifier));
            AddToBuffer(&error, sizeof(task_errors));

            size_t messageIndex = sizeof(USBController::USBMessageIdentifier) + sizeof(task_errors);
            PadBuffer(messageIndex);
        }
    }
}

void USBController::PadBuffer(size_t messageIndex) {
	size_t paddingSize = _maxMsgSize - messageIndex;
	memset(_txBuffer + _txBufferIndex, 0, paddingSize); // Pads with zeros
	_txBufferIndex += paddingSize;
}

void USBController::AddToBuffer(void* msg, size_t msgSize) {
	memcpy(_txBuffer + _txBufferIndex, msg, msgSize); 
	_txBufferIndex += msgSize;
}


bool USBController::BlockAndSendIfBufferFull()
{
    if (_txBufferIndex + _maxMsgSize >= USB_TX_BUFFER_SIZE) {
        while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
            osDelay(1);
        }
        _txBufferIndex = 0;
        
    }

	return true;

}

extern "C" void usbcontroller_main(osMessageQueueId_t sessionControllerToBpmHandle)
{
	USBController usb = USBController(sessionControllerToBpmHandle);

	if (!usb.Init())
	{
		return;
	}

	usb.Run();
}
