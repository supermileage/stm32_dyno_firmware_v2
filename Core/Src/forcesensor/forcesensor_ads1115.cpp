#include <forcesensor/forcesensor_ads1115.h>

#define MAX_FORCE_LBF 25
#define LBF_TO_NEWTON 4.44822

// Global interrupts
volatile bool ads1115_alert_status = false;

class ForcesensorADS1115 /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		ForcesensorADS1115(I2C_HandleTypeDef* i2cHandle,
                TIM_HandleTypeDef* timestampTimer,
				osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle);
		virtual ~ForcesensorADS1115() = default;

		bool Init();
		void Run();

	private:
		float GetForce(void);

		I2C_HandleTypeDef* _i2cHandle;
        TIM_HandleTypeDef* _timer;

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;

};

ForcesensorADS1115::ForcesensorADS1115(I2C_HandleTypeDef* i2cHandle,
                TIM_HandleTypeDef* timestampTimer,
				osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle) : /* Constructor */
		_i2cHandle(i2cHandle),
        _timer(timestampTimer),
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle),
		_forceSensorToSessionControllerHandle(forceSensorToSessionControllerHandle)
{}

bool ForcesensorADS1115::Init()
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
            outputData.timestamp = __HAL_TIM_GET_COUNTER(_timer);

            // Send to session controller
            osMessageQueuePut(_forceSensorToSessionControllerHandle, &outputData, 0, 0);
        }
        
    }
}



float ForcesensorADS1115::GetForce(void)
{
    // Do not poll, we will explicitly trigger the conversion
    uint16_t adcValue = ADS1115_getConversion(false);
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
