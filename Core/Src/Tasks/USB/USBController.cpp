#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USBController.hpp>
#include <MessagePassing/circular_buffers.h>

#include <CircularBufferReader.hpp>

USBController::USBController()
    : _buffer_reader_oe(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_fs(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _oe_output{0},
      _fs_output{0},
      _bpm_output{0},
      _txBuffer{0},
      _txBufferIndex(0)
{}

bool USBController::Init()
{
	return true;
}

void USBController::Run()
{

	while(1)
	{
		while (_buffer_reader_oe.GetElementAndIncrementIndex(_oe_output)) { // Takes in struct but only passes in address
			memcpy(_txBuffer + _txBufferIndex, &_oe_output, sizeof(_oe_output)); // Pointer arithmetic
			USBController::SafeIncrement(sizeof(_oe_output)); // Will fill as many bytes as are in the struct going into _txBuffer
		}
		
		while (_buffer_reader_fs.GetElementAndIncrementIndex(_fs_output)) {
			memcpy(_txBuffer + _txBufferIndex, &_fs_output, sizeof(_fs_output));
			USBController::SafeIncrement(sizeof(_fs_output));
		}

		while (_buffer_reader_bpm.GetElementAndIncrementIndex(_bpm_output)) {
			memcpy(_txBuffer + _txBufferIndex, &_bpm_output, sizeof(_bpm_output));
			USBController::SafeIncrement(sizeof(_bpm_output));
		}

		if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
			osDelay(1);
			continue;
		}
		_txBufferIndex = 0;
	}

}

void USBController::SafeIncrement(size_t addSize) { // takes in size that it's meant to progress length of _txBuffer by
	if (_txBufferIndex + sizeof(addSize) >= APP_RX_DATA_SIZE) {
		// Transmit to USB when safe
		while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) { // This line writes to USB device once the flag returns false
			osDelay(1);
		}
		_txBufferIndex = 0; // Reset buffer index (actual buffer not cleared, just freed for overwriting)
	} else {
		_txBufferIndex = _txBufferIndex + sizeof(addSize);
		// Must progress through txBuffer to account for its indices' size being smaller than that of each struct it contains
	}
}

extern "C" void usb_main()
{
	USBController usb = USBController();

	if (!usb.Init())
	{
		return;
	}

	usb.Run();
}
