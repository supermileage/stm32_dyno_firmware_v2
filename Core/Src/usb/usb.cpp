#include <usb/usb.h>

class USB
{
	public:
		USB();
		~USB() = default;

		bool Init();
		void Run();
	private:

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



