#include <forcesensor/forcesensor_adc.h>

#define MAX_FORCE 25
#define LBF_TO_NEWTON 4.44822

// Global interrupts
volatile uint32_t timestamp = 0;
volatile uint16_t adc_value = 0;

class ForcesensorADC /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		ForcesensorADC(osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle,
				ADC_HandleTypeDef* adcHandle);
		virtual ~ForcesensorADC() = default;

		bool Init();
		void Run();

	private:
		float GetForce(uint16_t adcValue);

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;
		osMessageQueueId_t _adcCallbackHandle;

		ADC_HandleTypeDef* _adcHandle;

};

ForcesensorADC::ForcesensorADC(osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle,
				ADC_HandleTypeDef* adcHandle) : /* Constructor */
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle),
		_forceSensorToSessionControllerHandle(forceSensorToSessionControllerHandle),
		_adcHandle(adcHandle)
{}

bool ForcesensorADC::Init()
{
	return true;
}

void ForcesensorADC::Run(void)
{
	bool enableADC = false;
	forcesensor_adc_to_session_controller outputData;

	while (1)
	{
	    osMessageQueueGet(_sessionControllerToForceSensorHandle, &enableADC, NULL, 0);
		if (enableADC) {
			HAL_ADC_Start_IT(_adcHandle); // Enables interrupt callback

			outputData.timestamp = timestamp;
			outputData.force = GetForce(adc_value);

			osMessageQueuePut(_forceSensorToSessionControllerHandle, &outputData, 0, 0);
		}
	}

}



float ForcesensorADC::GetForce(uint16_t adcValue)
{
	return static_cast<float> (adcValue) / UINT16_MAX * MAX_FORCE * LBF_TO_NEWTON; // Have the calculation here
}


extern "C" void adc_forcesensor_interrupt(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* timer)
{
	timestamp = __HAL_TIM_GET_COUNTER(timer);
	adc_value = HAL_ADC_GetValue(hadc);
}

extern "C" void force_sensor_adc_main(osMessageQueueId_t sessionControllerToForceSensorADCHandle, osMessageQueueId_t forceSensorADCToSessionControllerHandle, ADC_HandleTypeDef* adcHandle)
{
	ForcesensorADC forcesensor = ForcesensorADC(sessionControllerToForceSensorADCHandle, forceSensorADCToSessionControllerHandle, adcHandle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
