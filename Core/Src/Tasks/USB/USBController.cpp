#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USBController.hpp>

#include <CircularBufferReader.hpp>

bool USBController::Init()
{
	return true;
}

void USBController::Run()
{
	optical_encoder_output_data oe_output;
	forcesensor_output_data fs_output;
	bpm_output_data bpm_output;
	
	while(1)
	{
		_buffer_reader_oe.GetElementAndIncrementIndex(oe_output);
		_buffer_reader_fs.GetElementAndIncrementIndex(fs_output);
		_buffer_reader_bpm.GetElementAndIncrementIndex(bpm_output);
		osDelay(1);
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
