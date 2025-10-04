#include <xqueue.h>


QueueHandle_t sessionControllerToLumexLCDqHandle;

bool InitAllQueues()
{
	sessionControllerToLumexLCDqHandle = xQueueCreate(10, sizeof(session_controller_to_lumex_lcd*));

	if (sessionControllerToLumexLCDqHandle == NULL)
	{

		return false;
	}

	return true;
}
