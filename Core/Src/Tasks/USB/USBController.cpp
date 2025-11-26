#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USBController.hpp>

#include <CircularBufferReader.hpp>

bool USBController::Init()
{
	return true;
}

void USBController::Run()
{

	while(1)
	{
		if (_buffer_reader_oe.GetElementAndIncrementIndex(oe_output)) { // Takes in struct but only passes in address
			memcpy(_txBuffer + _txBufferIndex, &oe_output, sizeof(oe_output));
			USBController::SafeIncrement(sizeof(oe_output));
		}
		
		if (_buffer_reader_fs.GetElementAndIncrementIndex(fs_output)) {
			memcpy(_txBuffer + _txBufferIndex, &fs_output, sizeof(fs_output));
			USBController::SafeIncrement(sizeof(fs_output));
		}

		if (_buffer_reader_bpm.GetElementAndIncrementIndex(bpm_output)) {
			memcpy(_txBuffer + _txBufferIndex, &bpm_output, sizeof(bpm_output));
			USBController::SafeIncrement(sizeof(bpm_output));
		}
		osDelay(1);
	}

}

void USBController::SafeIncrement(size_t addSize) {
	if (_txBufferIndex + sizeof(oe_output) >= USB_CDC_TX_BUFFER_SIZE) {
		// Transmit to USB when safe
		while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
			osDelay(1);
		}
		_txBufferIndex = 0;
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
