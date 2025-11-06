#include <opticalsensor/opticalsensor.h>

class OpticalSensorADC /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		OpticalSensorADC(ADC_HandleTypeDef* adcHandle,
				osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle);
		virtual ~OpticalSensorADC() = default;

		bool Init();
		void Run();

	private:
		float GetForce(uint16_t adcValue);

		ADC_HandleTypeDef* _adcHandle;
		osMessageQueueId_t _sessionControllerToopticalSensorHandle;
		osMessageQueueId_t _opticalSensorToSessionControllerHandle;

};

OpticalSensorADC::OpticalSensorADC(ADC_HandleTypeDef* adcHandle,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle) : /* Constructor */
		_adcHandle(adcHandle),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_opticalSensorToSessionControllerHandle(opticalSensorToSessionControllerHandle)
{}

bool OpticalSensorADC::Init()
{
	return true;
}

void OpticalSensorADC::Run(void)
{
	bool enableADC = false;
	optical_sensor_output_data outputData;

	while (1)
	{
	    osMessageQueueGet(_sessionControllerToOpticalSensorHandle, &enableADC, NULL, 0);
		if (enableADC) {
			HAL_ADC_Start_IT(_adcHandle); // Enables interrupt callback
			outputData.timestamp = timestamp;
			outputData.force = GetForce(adc_value);

			osMessageQueuePut(_opticalSensorToSessionControllerHandle, &outputData, 0, 0);
		}
	}
}


extern "C" void optical_sensor_interrupt(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* timer)
{
	timestamp = __HAL_TIM_GET_COUNTER(timer);
	adc_value = HAL_ADC_GetValue(hadc);
}

extern "C" void optical_sensor_main(ADC_HandleTypeDef* adcHandle, osMessageQueueId_t sessionControllerToOpticalSensorADCHandle, osMessageQueueId_t OpticalSensorADCToSessionControllerHandle)
{
	OpticalSensorADC opticalsensor = OpticalSensorADC(adcHandle, sessionControllerToOpticalSensorADCHandle, OpticalSensorADCToSessionControllerHandle);

	if (!opticalsensor.Init())
	{
		return;
	}

	while(1)
	{
		opticalsensor.Run();
	}
}
