#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USBController.hpp>
#include <MessagePassing/circular_buffers.h>

#include <CircularBufferReader.hpp>

#include <iostream>

using namespace std;

USBController::USBController(osMessageQueueId_t sessionControllerToUsbController)
    : _buffer_reader_oe(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_fs(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _oe_output{0},
      _fs_output{0},
      _bpm_output{0},
	  _sessionControllerToUsbController(sessionControllerToUsbController),
	  _standardSize{0},
      _txBuffer{0},
      _txBufferIndex(0)
{}

bool USBController::Init()
{
	// Need to pad every struct to the largest size out of all of them
	_standardSize = std::max(sizeof(_oe_output), std::max(sizeof(_bpm_output), sizeof(_fs_output)));

	return true;
}

void USBController::Run()
{
	bool enableUSB = false;

	send_usb_output_data opEcdr = OPTICAL_ENCODER;
	send_usb_output_data fcSnsr = FORCESENSOR;
	send_usb_output_data bkPowm = BRAKE_PWM;

	while(1)
	{
		osMessageQueueGet(_sessionControllerToUsbController, &enableUSB, NULL, 0);
		if (enableUSB) {
			while (!SendOutputIfBufferFull(sizeof(opEcdr), _standardSize) && _buffer_reader_oe.GetElementAndIncrementIndex(_oe_output)) { // Takes in struct but only passes in address
				AddToBuffer(&opEcdr, sizeof(opEcdr));
				AddToBuffer(&_oe_output, _standardSize);
			}

			while (!SendOutputIfBufferFull(sizeof(fcSnsr), _standardSize) && _buffer_reader_fs.GetElementAndIncrementIndex(_fs_output)) {
				AddToBuffer(&fcSnsr, sizeof(fcSnsr));
				AddToBuffer(&_fs_output, _standardSize);
			}

			while (!SendOutputIfBufferFull(sizeof(bkPowm), _standardSize) && _buffer_reader_bpm.GetElementAndIncrementIndex(_bpm_output)) {
				AddToBuffer(&bkPowm, sizeof(bkPowm));
				AddToBuffer(&_bpm_output, _standardSize);
				_txBufferIndex = _txBufferIndex + sizeof(bkPowm) + sizeof(_bpm_output);
			}

			if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
	
				continue;
			}
			_txBufferIndex = 0;
		}
	}

}

//size_t USBController::EnoughSpace(size_t enumSend, size_t outputSend) {
//	return TX_BUFFER_SIZE > _txBufferIndex + enumSend + outputSend;
//}

void USBController::AddToBuffer(void* outputData, size_t outputDataSize) {
	memcpy(_txBuffer + _txBufferIndex, outputData, outputDataSize); // Adds
	_txBufferIndex += outputDataSize;
}

bool USBController::SendOutputIfBufferFull(size_t enumSize, size_t outputSize)
{
if (_txBufferIndex + sizeof(outputSize) >= TX_BUFFER_SIZE) {
		// Transmit to USB when safe
		while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) { // This line writes to USB device once the flag returns false
			osDelay(1);
		}
		_txBufferIndex = 0; // Reset buffer index (actual buffer not cleared, just freed for overwriting)
		return true;
	} 

	return false;
}

extern "C" void usb_main(osMessageQueueId_t sessionControllerToBpmHandle)
{
	USBController usb = USBController(sessionControllerToBpmHandle);

	if (!usb.Init())
	{
		return;
	}

	usb.Run();
}
