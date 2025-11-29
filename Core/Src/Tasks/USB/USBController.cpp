#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USBController.hpp>
#include <MessagePassing/circular_buffers.h>

#include <CircularBufferReader.hpp>

USBController::USBController(osMessageQueueId_t sessionControllerToUsbController)
    : _buffer_reader_oe(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_fs(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _oe_output{0},
      _fs_output{0},
      _bpm_output{0},
	  _sessionControllerToUsbController(sessionControllerToUsbController),
      _txBuffer{0},
      _txBufferIndex(0)
{}

bool USBController::Init()
{
	return true;
}

void USBController::Run()
{
	bool enableUSB = false;

	send_usb_output_data opEcdr = OPTICAL_ENCODER;
	send_usb_output_data fcSnsr = FORCESENSOR;
	send_usb_output_data bkPowm = BPM;

	while(1)
	{
		osMessageQueueGet(_sessionControllerToUsbController, &enableUSB, NULL, 0);
		if (enableUSB) {
			while (EnoughSpace(sizeof(opEcdr), sizeof(_oe_output)) && _buffer_reader_oe.GetElementAndIncrementIndex(_oe_output)) { // Takes in struct but only passes in address
				memcpy(_txBuffer + _txBufferIndex, &opEcdr, sizeof(opEcdr));
				memcpy(_txBuffer + _txBufferIndex, &_oe_output, sizeof(_oe_output)); // Pointer arithmetic
				_txBufferIndex = _txBufferIndex + sizeof(opEcdr) + sizeof(_oe_output);
				SendOutputToUSB(sizeof(_oe_output));
			}

			while (EnoughSpace(sizeof(fcSnsr), sizeof(_fs_output)) && _buffer_reader_fs.GetElementAndIncrementIndex(_fs_output)) {
				memcpy(_txBuffer + _txBufferIndex, &fcSnsr, sizeof(fcSnsr));
				memcpy(_txBuffer + _txBufferIndex, &_fs_output, sizeof(_fs_output));
				_txBufferIndex = _txBufferIndex + sizeof(fcSnsr) + sizeof(_fs_output);
				SendOutputToUSB(sizeof(_fs_output));
			}

			while (EnoughSpace(sizeof(bkPowm), sizeof(_bpm_output)) && _buffer_reader_bpm.GetElementAndIncrementIndex(_bpm_output)) {
				memcpy(_txBuffer + _txBufferIndex, &bkPowm, sizeof(bkPowm));
				memcpy(_txBuffer + _txBufferIndex, &_bpm_output, sizeof(_bpm_output));
				_txBufferIndex = _txBufferIndex + sizeof(bkPowm) + sizeof(_bpm_output);
				SendOutputToUSB(sizeof(_bpm_output));
			}

			if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
	
				continue;
			}
			_txBufferIndex = 0;
		}
	}

}

size_t USBController::EnoughSpace(size_t enumSend, size_t outputSend) {
	return TX_BUFFER_SIZE > _txBufferIndex + enumSend + outputSend;
}

void USBController::SendOutputToUSB(size_t addSize) { // takes in size that it's meant to progress length of _txBuffer by
	if (_txBufferIndex + sizeof(addSize) >= TX_BUFFER_SIZE) {
		// Transmit to USB when safe
		while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) { // This line writes to USB device once the flag returns false
			osDelay(1);
		}
		_txBufferIndex = 0; // Reset buffer index (actual buffer not cleared, just freed for overwriting)
	} 
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
