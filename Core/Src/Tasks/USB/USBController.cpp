#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USBController.hpp>
#include <MessagePassing/messages.h>

#include <CircularBufferReader.hpp>

USBController::USBController()
    : _buffer_reader_oe(optical_encoder_circular_buffer, APP_TX_DATA_SIZE, optical_encoder_circular_buffer_index_writer),
      _buffer_reader_fs(forcesensor_circular_buffer, APP_TX_DATA_SIZE, forcesensor_circular_buffer_index_writer),
      _buffer_reader_bpm(bpm_circular_buffer, APP_TX_DATA_SIZE, bpm_circular_buffer_index_writer),
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
		while (_buffer_reader_oe.GetElementAndIncrementIndex(oe_output)) { // Takes in struct but only passes in address
			memcpy(_txBuffer + _txBufferIndex, &oe_output, sizeof(oe_output)); // Pointer arithmetic
			USBController::SafeIncrement(sizeof(oe_output)); // Will fill as many bytes as are in the struct going into _txBuffer
		}
		
		while (_buffer_reader_fs.GetElementAndIncrementIndex(fs_output)) {
			memcpy(_txBuffer + _txBufferIndex, &fs_output, sizeof(fs_output));
			USBController::SafeIncrement(sizeof(fs_output));
		}

		while (_buffer_reader_bpm.GetElementAndIncrementIndex(bpm_output)) {
			memcpy(_txBuffer + _txBufferIndex, &bpm_output, sizeof(bpm_output));
			USBController::SafeIncrement(sizeof(bpm_output));
		}

		if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
			osDelay(1);
			continue;
		}
		_txBufferIndex = 0;
	}

}

void USBController::SafeIncrement(size_t addSize) { // takes in size that it's meant to progress length of _txBuffer by
	if (_txBufferIndex + sizeof(oe_output) >= APP_RX_DATA_SIZE) {
		// Transmit to USB when safe
		while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) { // This line writes to USB device once the flag returns false
			osDelay(1);
		}
		_txBufferIndex = 0; // Reset buffer index (actual buffer not cleared, just freed for overwriting)
	} else {
		_txBufferIndex = _txBufferIndex + sizeof(oe_output);
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
