#include <Tasks/USB/usbcontroller_main.h>
#include <Tasks/USB/USBController.hpp>

USBController::USBController(osMessageQueueId_t sessionControllerToUsbController)
    : _buffer_reader_oe(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_fs(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _opticalEncoderOutput{},
      _forceSensorOutput{},
      _bpmOutput{},
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



	while(1)
	{
		osMessageQueueGet(_sessionControllerToUsbController, &enableUSB, NULL, 0);
		if (enableUSB) {
			while (!SendOutputIfBufferFull(sizeof(USBOpcode), sizeof(_opticalEncoderOutput)) && _buffer_reader_oe.GetElementAndIncrementIndex(_opticalEncoderOutput)) { // Takes in struct but only passes in address
				uint8_t op = static_cast<uint8_t>(USBOpcode::OPTICAL_ENCODER);
				AddToBuffer(&op, sizeof(USBOpcode));
				AddToBuffer(&_opticalEncoderOutput, sizeof(_opticalEncoderOutput));
			}

			while (!SendOutputIfBufferFull(sizeof(USBOpcode), sizeof(_forceSensorOutput)) && _buffer_reader_fs.GetElementAndIncrementIndex(_forceSensorOutput)) {
				uint8_t op = static_cast<uint8_t>(USBOpcode::FORCESENSOR);
				AddToBuffer(&op, sizeof(USBOpcode));
				AddToBuffer(&_forceSensorOutput, sizeof(_forceSensorOutput));
			}

			while (!SendOutputIfBufferFull(sizeof(USBOpcode), sizeof(_bpmOutput)) && _buffer_reader_bpm.GetElementAndIncrementIndex(_bpmOutput)) {
				uint8_t op = static_cast<uint8_t>(USBOpcode::BPM);
				AddToBuffer(&op, sizeof(USBOpcode));
				AddToBuffer(&_bpmOutput, sizeof(_bpmOutput));
			}

			if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
	
				continue;
			}
			_txBufferIndex = 0;
		}

		osDelay(USB_TASK_OSDELAY);
	}

}

void USBController::AddToBuffer(void* outputData, size_t outputDataSize) {
	memcpy(_txBuffer + _txBufferIndex, outputData, outputDataSize); // Adds
	_txBufferIndex += outputDataSize;
}

bool USBController::SendOutputIfBufferFull(size_t enumSize, size_t outputSize)
{
    if (_txBufferIndex + enumSize + outputSize >= USB_TX_BUFFER_SIZE) {
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