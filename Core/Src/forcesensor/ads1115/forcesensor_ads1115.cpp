#include <forcesensor/ads1115/forcesensor_ads1115_main.h>
#include <forcesensor/ads1115/forcesensor_ads1115.hpp>

#define MAX_FORCE_LBF 25
#define LBF_TO_NEWTON 4.44822

// Global interrupts
volatile bool ads1115_alert_status = false;

ForcesensorADS1115::ForcesensorADS1115(I2C_HandleTypeDef* i2cHandle,
                TIM_HandleTypeDef* timestampTimer,
				osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle) :
		_i2cHandle(i2cHandle),
        _timer(timestampTimer),
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle),
		_forceSensorToSessionControllerHandle(forceSensorToSessionControllerHandle),
		_ads1115(_i2cHandle)
{}

bool ForcesensorADS1115::Init()
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

void ForcesensorADS1115::Run(void)
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
            outputData.force = GetForce();

            // Capture timestamp once value is received
            outputData.timestamp = __HAL_TIM_GET_COUNTER(_timer);

            // Send to session controller
            osMessageQueuePut(_forceSensorToSessionControllerHandle, &outputData, 0, 0);
        }
        
    }
}



float ForcesensorADS1115::GetForce(void)
{
    // Do not poll, we will explicitly trigger the conversion
    uint16_t adcValue = _ads1115.getConversion(false);
    return static_cast<float>(adcValue) / UINT16_MAX * MAX_FORCE_LBF * LBF_TO_NEWTON;
}



extern "C" void force_sensor_ads1115_gpio_alert_interrupt(void)
{
    ads1115_alert_status = true;
}

extern "C" void force_sensor_ads1115_main(I2C_HandleTypeDef* i2cHandle, TIM_HandleTypeDef* timestampTimer, osMessageQueueId_t sessionControllerToForcesensorADS1115Handle, osMessageQueueId_t ForcesensorADS1115ToSessionControllerHandle)
{
	ForcesensorADS1115 forcesensor = ForcesensorADS1115(i2cHandle, timestampTimer, sessionControllerToForcesensorADS1115Handle, ForcesensorADS1115ToSessionControllerHandle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
