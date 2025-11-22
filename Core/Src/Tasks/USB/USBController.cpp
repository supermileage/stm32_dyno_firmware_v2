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
