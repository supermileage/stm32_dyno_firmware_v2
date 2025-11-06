#include <opticalsensor/opticalsensor.h>

volatile uint32_t timestamp_os = 0;
volatile float rpm = 0;

class OpticalSensorADC /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		OpticalSensorADC(ADC_HandleTypeDef* adcHandle,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle);
		virtual ~OpticalSensorADC() = default;

		bool Init();
		void Run();

	private:
		float GetRPM(uint16_t adcValue);

		ADC_HandleTypeDef* _adcHandle;
		osMessageQueueId_t _sessionControllerToOpticalSensorHandle;
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
	optical_encoder_output_data outputData;

	while (1)
	{
	    osMessageQueueGet(_sessionControllerToOpticalSensorHandle, &enableADC, NULL, 0);
		if (enableADC) {
			HAL_ADC_Start_IT(_adcHandle); // Enables interrupt callback
			outputData.timestamp_os = timestamp_os;
			outputData.rpm = GetRPM(rpm);

			osMessageQueuePut(_opticalSensorToSessionControllerHandle, &outputData, 0, 0);
		}
	}
}

float OpticalSensorADC::GetRPM(uint16_t adcValue)
{
	return rpm; // Current implementation takes in adc_value but doesn't actually use it. Change?
}


extern "C" void optical_sensor_interrupt(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef* timer)
{
	timestamp_os = __HAL_TIM_GET_COUNTER(timer);
	rpm = HAL_ADC_GetValue(hadc);
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
