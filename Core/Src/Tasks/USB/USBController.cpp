#include <Tasks/USB/usbcontroller_main.h>
#include <Tasks/USB/USBController.hpp>

#include <iostream>
using namespace std;

USBController::USBController(osMessageQueueId_t sessionControllerToUsbController)
    : _buffer_reader_oe(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_fs(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _opticalEncoderOutput{},
      _forceSensorOutput{},
      _bpmOutput{},
	  _standardSize{0},
	  _sessionControllerToUsbController(sessionControllerToUsbController),
      _txBuffer{},
      _txBufferIndex(0)
{}

bool USBController::Init()
{
	_standardSize = std::max(std::max(sizeof(USBOpcode) + sizeof(_opticalEncoderOutput), sizeof(USBOpcode) + sizeof(_bpmOutput)), sizeof(USBOpcode) + sizeof(_forceSensorOutput));
	
	return true;
}

void USBController::Run()
{
	bool enableUSB = false;

	while(1)
	{
		osMessageQueueGet(_sessionControllerToUsbController, &enableUSB, NULL, 0);
		if (enableUSB) {
			while (!SendOutputIfBufferFull(sizeof(USBOpcode), sizeof(_standardSize)) && _buffer_reader_oe.GetElementAndIncrementIndex(_opticalEncoderOutput)) { // Takes in struct but only passes in address
				uint8_t op = static_cast<uint8_t>(USBOpcode::OPTICAL_ENCODER);
				AddToBuffer(&op, sizeof(USBOpcode));
				AddToBuffer(&_opticalEncoderOutput, sizeof(_opticalEncoderOutput), _standardSize);
			}

			while (!SendOutputIfBufferFull(sizeof(USBOpcode), sizeof(_standardSize)) && _buffer_reader_fs.GetElementAndIncrementIndex(_forceSensorOutput)) {
				uint8_t op = static_cast<uint8_t>(USBOpcode::FORCESENSOR);
				AddToBuffer(&op, sizeof(USBOpcode));
				AddToBuffer(&_forceSensorOutput, sizeof(_forceSensorOutput), _standardSize);
			}

			while (!SendOutputIfBufferFull(sizeof(USBOpcode), sizeof(_standardSize)) && _buffer_reader_bpm.GetElementAndIncrementIndex(_bpmOutput)) {
				uint8_t op = static_cast<uint8_t>(USBOpcode::BPM);
				AddToBuffer(&op, sizeof(USBOpcode));
				AddToBuffer(&_bpmOutput, sizeof(_bpmOutput), _standardSize);
			}

			if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
				continue;
			}
			_txBufferIndex = 0;
		}
	}
}

void USBController::AddToBuffer(void* outputType, size_t outputTypeSize) {
	memcpy(_txBuffer + _txBufferIndex, outputType, outputTypeSize); // Adds
	_txBufferIndex += outputTypeSize;
}

void USBController::AddToBuffer(void* messageData, size_t actualMessageSize, size_t totalExpectedMessageSize)
{
    memcpy(_txBuffer + _txBufferIndex, messageData, actualMessageSize);

    if (actualMessageSize < totalExpectedMessageSize) { // Checks if padding is necessary for data to match _standardSize
        memset(_txBuffer + _txBufferIndex + actualMessageSize, 0, totalExpectedMessageSize - actualMessageSize); // Sets every byte from actualSize to outputDataSize equal to NULL
    }

    // 3. Increment index by the full message size
    _txBufferIndex += totalExpectedMessageSize;
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