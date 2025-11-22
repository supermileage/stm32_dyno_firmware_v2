#include <Tasks/USB/usb_main.h>
#include <Tasks/USB/USB.hpp>

#define CDC_TX_BUFFER_SIZE 256

class USB
{
	public:
		USB();
		~USB() = default;

		bool Init();
		void Run();
	private:
		uint8_t _txBuffer[CDC_TX_BUFFER_SIZE];

};

bool USB::Init()
{
	return true;
}

void USB::Run()
{
	while(1)
	{

	}

}

extern "C" void usb_main()
{
	USB usb = USB();

	if (!usb.Init())
	{
		return;
	}

	usb.Run();
}
