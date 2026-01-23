#include <Tasks/ForceSensor/ADS1115/forcesensor_ads1115_main.h>
#include <Tasks/ForceSensor/ADS1115/ForceSensor_ADS1115.hpp>

#define LBF_TO_NEWTON 4.44822

// Global interrupts
static volatile bool ads1115_alert_status = false;

ForceSensorADS1115::ForceSensorADS1115(osMessageQueueId_t sessionControllerToForceSensorHandle) : 
		// this comes directly from circular_buffers.hpp and config.h
		_buffer_writer(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
        _ads1115(forceSensorADS1115Handle),
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle) {}

bool ForceSensorADS1115::Init()
{
	_ads1115.initialize();

    // Set device mode to single-shot
	_ads1115.setMode(ADS1115_MODE_SINGLESHOT);

    // Set data rate (slow for demonstration or high depending on application)
	_ads1115.setRate(ADS1115_SAMPLE_SPEED);

    // Set PGA (programmable gain amplifier)
	_ads1115.setGain(ADS1115_PGA_6P144);

	_ads1115.setConversionReadyPinMode();
    
    return true;
}

void ForceSensorADS1115::Run(void)
{
    bool enableADS1115 = false;
    forcesensor_output_data outputData;

    while (1)
    {
        // Check if new message from session controller
        osMessageQueueGet(_sessionControllerToForceSensorHandle, &enableADS1115, NULL, 0);

        if (enableADS1115)
        {
            // Trigger conversion
        	_ads1115.triggerConversion();

            // Wait for alert GPIO to indicate conversion complete
            while (!ads1115_alert_status)
            {
                osDelay(1); // optional: yield to other tasks
            }
            ads1115_alert_status = false;

            // Read the conversion
            uint16_t rawVal = _ads1115.getConversion(false);
            outputData.force = GetForce(rawVal);

            // Capture timestamp once value is received
            outputData.timestamp = get_timestamp();

            outputData.raw_value = rawVal;

            // Add to circular buffer
            _buffer_writer.WriteElementAndIncrementIndex(outputData);
        }
        
    }
}



float ForceSensorADS1115::GetForce(uint16_t raw_value)
{
    // Do not poll, we will explicitly trigger the conversion
    return static_cast<float>(raw_value) / UINT16_MAX * MAX_FORCE_LBF * LBF_TO_NEWTON;
}



extern "C" void forcesensor_ads1115_gpio_alert_interrupt(void)
{
    ads1115_alert_status = true;
}

extern "C" void forcesensor_ads1115_main(osMessageQueueId_t sessionControllerToForcesensorADS1115Handle)
{
	ForceSensorADS1115 forcesensor = ForceSensorADS1115(sessionControllerToForcesensorADS1115Handle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
