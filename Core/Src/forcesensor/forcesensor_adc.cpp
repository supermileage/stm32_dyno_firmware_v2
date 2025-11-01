#include <forcesensor/forcesensor_adc.h>

#define SIXTEEN_BIT_MAX 65535
#define MAX_FORCE 25
#define LBF_TO_NEWTON 4.44822

class ForcesensorADC /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		ForcesensorADC(osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle,
				osMessageQueueId_t adcCallbackHandle,
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
				osMessageQueueId_t adcCallbackHandle,
				ADC_HandleTypeDef* adcHandle) : /* Constructor */
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle),
		_forceSensorToSessionControllerHandle(forceSensorToSessionControllerHandle),
		_adcCallbackHandle(adcCallbackHandle),
		_adcHandle(adcHandle)
{}

bool ForcesensorADC::Init()
{
	return true;
}

void ForcesensorADC::Run(void)
{
	bool enableADC = false;
	adc_callback_to_forcesensor callbackData; // data coming from the forcesensor interrupt
	forcesensor_adc_to_session_controller outputData;

	while (1)
	{
	    if (enableADC) {
			HAL_ADC_Start_IT(_adcHandle); // Enables interrupt callback
			osMessageQueueGet(_adcCallbackHandle, &callbackData, 0, osWaitForever); // osWaitForever is an "enum"
			outputData.timestamp = callbackData.timestamp;
			outputData.force = GetForce(callbackData.adc_value);

			osMessageQueuePut(_forceSensorToSessionControllerHandle, &outputData, 0, 0);
		}
	}

}



float ForcesensorADC::GetForce(uint16_t adcValue)
{
	return static_cast<float> (adcValue) / SIXTEEN_BIT_MAX * MAX_FORCE * LBF_TO_NEWTON; // Have the calculation here
}


extern "C" void adc_forcesensor_interrupt(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* timer,  osMessageQueueId_t osHandle)
{
	adc_callback_to_forcesensor msg;
	msg.timestamp = __HAL_TIM_GET_COUNTER(timer);
	msg.adc_value = HAL_ADC_GetValue(hadc);
	osMessageQueuePut(osHandle, &msg, 0, 0); // priority and timeout are last two numbers
}

extern "C" void force_sensor_adc_main(osMessageQueueId_t sc_to_fsHandle, osMessageQueueId_t fs_to_scHandle, osMessageQueueId_t adcCallbackHandle, ADC_HandleTypeDef* adcHandle)
{
	ForcesensorADC forcesensor = ForcesensorADC(sc_to_fsHandle, fs_to_scHandle, adcCallbackHandle, adcHandle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
