#include <Tasks/ForceSensor/ADC/forcesensor_adc_main.h>
#include <Tasks/ForceSensor/ADC/ForceSensor_ADC.hpp>

#define MAX_FORCE_LBF 25
#define LBF_TO_NEWTON 4.44822

// Global interrupts
volatile uint32_t timestamp = 0;
volatile uint16_t adc_value = 0;

ForceSensorADC::ForceSensorADC(ADC_HandleTypeDef* adcHandle,
				osMessageQueueId_t sessionControllerToForceSensorHandle) : /* Constructor */
		_buffer_writer(forcesensor_circular_buffer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
		_adcHandle(adcHandle),
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle)
{}

bool ForceSensorADC::Init()
{
	return true;
}

void ForceSensorADC::Run(void)
{
	bool enableADC = false;
	forcesensor_output_data outputData;

	while (1)
	{
	    osMessageQueueGet(_sessionControllerToForceSensorHandle, &enableADC, NULL, 0);
		if (enableADC) {
			HAL_ADC_Start_IT(_adcHandle); // Enables interrupt callback

			outputData.timestamp = timestamp;
			outputData.force = GetForce(adc_value);

			// Add to circular buffer
            _buffer_writer.WriteElementAndIncrementIndex(outputData);
		}
	}

}



float ForceSensorADC::GetForce(uint16_t adcValue)
{
	return static_cast<float> (adcValue) / UINT16_MAX * MAX_FORCE_LBF * LBF_TO_NEWTON; // Have the calculation here
}


extern "C" void adc_forcesensor_interrupt(ADC_HandleTypeDef* hadc)
{
	timestamp = get_timestamp();
	adc_value = HAL_ADC_GetValue(hadc);
}

extern "C" void force_sensor_adc_main(ADC_HandleTypeDef* adcHandle, osMessageQueueId_t sessionControllerToForceSensorADCHandle)
{
	ForceSensorADC forcesensor = ForceSensorADC(adcHandle, sessionControllerToForceSensorADCHandle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
