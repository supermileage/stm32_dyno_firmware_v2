#include <Tasks/ForceSensor/ADS1115/forcesensor_ads1115_main.h>
#include <Tasks/ForceSensor/ADS1115/ForceSensor_ADS1115.hpp>

#define MAX_FORCE_LBF 25
#define LBF_TO_NEWTON 4.44822

// Global interrupts
volatile bool ads1115_alert_status = false;

ForceSensorADS1115::ForceSensorADS1115(I2C_HandleTypeDef* i2cHandle,
				osMessageQueueId_t sessionControllerToForceSensorHandle) : 
        _buffer_writer(forcesensor_circular_buffer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
		_i2cHandle(i2cHandle),
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle)
{}

bool ForceSensorADS1115::Init()
{
	ADS1115_initialize(_i2cHandle, ADS1115_DEFAULT_ADDRESS);

    // Set device mode to single-shot
    ADS1115_setMode(ADS1115_MODE_SINGLESHOT);

    // Set data rate (slow for demonstration or high depending on application)
    ADS1115_setRate(ADS1115_SAMPLE_SPEED);

    // Set PGA (programmable gain amplifier)
    ADS1115_setGain(ADS1115_PGA_6P144);

    ADS1115_setConversionReadyPinMode();
    
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
            ADS1115_triggerConversion();

            // Wait for alert GPIO to indicate conversion complete
            while (!ads1115_alert_status)
            {
                osDelay(1); // optional: yield to other tasks
            }
            ads1115_alert_status = false;

            // Read the conversion
            outputData.force = GetForce();

            // Capture timestamp once value is received
            outputData.timestamp = get_timestamp();

            // Add to circular buffer
            _buffer_writer.WriteElementAndIncrementIndex(outputData);
        }
        
    }
}



float ForceSensorADS1115::GetForce(void)
{
    // Do not poll, we will explicitly trigger the conversion
    uint16_t adcValue = ADS1115_getConversion(false);
    return static_cast<float>(adcValue) / UINT16_MAX * MAX_FORCE_LBF * LBF_TO_NEWTON;
}



extern "C" void force_sensor_ads1115_gpio_alert_interrupt(void)
{
    ads1115_alert_status = true;
}

extern "C" void force_sensor_ads1115_main(I2C_HandleTypeDef* i2cHandle, osMessageQueueId_t sessionControllerToForcesensorADS1115Handle)
{
	ForceSensorADS1115 forcesensor = ForceSensorADS1115(i2cHandle, sessionControllerToForcesensorADS1115Handle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
