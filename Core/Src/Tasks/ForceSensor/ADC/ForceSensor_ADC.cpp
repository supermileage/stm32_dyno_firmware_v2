#include <Tasks/ForceSensor/ADC/forcesensor_adc_main.h>
#include <Tasks/ForceSensor/ADC/ForceSensor_ADC.hpp>

#define LBF_TO_NEWTON 4.44822

// Global interrupts
static volatile uint32_t timestamp = 0;
static volatile uint16_t adc_value = 0;

ForceSensorADC::ForceSensorADC(osMessageQueueId_t sessionControllerToForceSensorHandle) :
		// this comes directly from circular_buffers.h and config.h
		_buffer_writer(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
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
        // --- Get the latest enable/disable state ---
        bool gotMessage = GetLatestFromQueue(_sessionControllerToForceSensorHandle,
                                             &enableADC,
                                             sizeof(enableADC),
                                             enableADC ? 0 : osWaitForever);

        // If no message received and ADC is disabled, just continue
        if (!gotMessage && !enableADC)
        {
            continue;
        }

        // Skip processing if the latest state says disabled
        if (!enableADC)
        {
            continue;
        }

        // --- Trigger ADC conversion via interrupt ---
        HAL_ADC_Start_IT(forceSensorADCHandle);

        // --- Populate output struct ---
        outputData.timestamp = timestamp;
        outputData.force = GetForce(adc_value);
        outputData.raw_value = adc_value;

        _buffer_writer.WriteElementAndIncrementIndex(outputData);

        // --- Yield to other tasks ---
        osDelay(FORCESENSOR_TASK_OSDELAY);
    }
}




float ForceSensorADC::GetForce(uint16_t adcValue)
{
	return static_cast<float> (adcValue) / UINT16_MAX * MAX_FORCE_LBF * LBF_TO_NEWTON; // Have the calculation here
}


extern "C" void forcesensor_adc_interrupt()
{
	timestamp = get_timestamp();
	adc_value = HAL_ADC_GetValue(forceSensorADCHandle);
}

extern "C" void forcesensor_adc_main(osMessageQueueId_t sessionControllerToForceSensorADCHandle)
{
	ForceSensorADC forcesensor = ForceSensorADC(sessionControllerToForceSensorADCHandle);

	if (!forcesensor.Init())
	{
		return;
	}

	while(1)
	{
		forcesensor.Run();
	}
}
