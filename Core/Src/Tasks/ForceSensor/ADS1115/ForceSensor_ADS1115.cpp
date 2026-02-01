#include <Tasks/ForceSensor/ADS1115/forcesensor_ads1115_main.h>
#include <Tasks/ForceSensor/ADS1115/ForceSensor_ADS1115.hpp>

#define LBF_TO_NEWTON 4.44822
#define ADS1115_VOLTAGE 5.1

extern I2C_HandleTypeDef* forceSensorADS1115Handle;


extern size_t forcesensor_circular_buffer_index_writer;
extern forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];

extern size_t task_error_circular_buffer_index_writer;
extern task_error_data task_error_circular_buffer[TASK_ERROR_CIRCULAR_BUFFER_SIZE];

// Global interrupts
static volatile bool ads1115_alert_status = false;

ForceSensorADS1115::ForceSensorADS1115(osMessageQueueId_t sessionControllerToForceSensorHandle) : 
		// this comes directly from circular_buffers.h and config.h
		_data_buffer_writer(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
        _task_error_buffer_writer(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
        _ads1115(forceSensorADS1115Handle),
		_sessionControllerToForceSensorHandle(sessionControllerToForceSensorHandle) {}

bool ForceSensorADS1115::Init()
{
    bool status = true;
    

    status &= _ads1115.setMultiplexer(ADS1115_MUX_P0_NG);

    status &= _ads1115.setComparatorMode(ADS1115_COMP_MODE_HYSTERESIS);
    status &= _ads1115.setComparatorPolarity(ADS1115_COMP_POL_ACTIVE_LOW);

    status &= _ads1115.setComparatorLatchEnabled(ADS1115_COMP_LAT_NON_LATCHING);
    status &= _ads1115.setComparatorQueueMode(ADS1115_COMP_QUE_DISABLE);

    
    // Set device mode to single-shot
	status &= _ads1115.setMode(ADS1115_MODE_SINGLESHOT);

    // Set data rate (slow for demonstration or high depending on application)
	status &= _ads1115.setRate(ADS1115_SAMPLE_SPEED);

    // Set PGA (programmable gain amplifier)
	status &= _ads1115.setGain(ADS1115_PGA_6P144);

	status &= _ads1115.setConversionReadyPinMode();

    if (!status)
    {
        task_error_data error_data = 
        {
            .timestamp = get_timestamp(),
            .task_id = TASK_ID_FORCE_SENSOR,
            .error_id = static_cast<uint32_t>(ERROR_FORCE_SENSOR_ADS1115_INIT_FAILURE)
        };
        _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
    }

    return status;
}

void ForceSensorADS1115::Run(void)
{
    bool enableADS1115 = false;
    forcesensor_output_data outputData;

    while (1)
    {
        // Get the latest enable/disable message
        // If currently disabled, block forever; if enabled, poll non-blocking
        GetLatestFromQueue(_sessionControllerToForceSensorHandle,
                                            &enableADS1115,
                                            sizeof(enableADS1115),
                                            enableADS1115 ? 0 : osWaitForever);

        // If the latest message says disabled, skip processing
        if (!enableADS1115)
        {
               continue;
        }

        ads1115_alert_status = false;

        // --- Trigger conversion ---
        if (!_ads1115.triggerConversion()) 
        {
            task_error_data error_data = 
            {
                .timestamp = get_timestamp(),
                .task_id = TASK_ID_FORCE_SENSOR,
                .error_id = static_cast<uint32_t>(WARNING_FORCE_SENSOR_ADS1115_TRIGGER_CONVERSION_FAILURE)
            };
            
            _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
            osDelay(TASK_WARNING_RETRY_OSDELAY);
            continue;
        }

        // --- Wait for alert GPIO to indicate conversion complete ---
        while (!ads1115_alert_status)
        {
            osDelay(1); // yield to other tasks
        }
    
        // --- Read conversion and populate output ---
        int16_t rawVal;
        if (!_ads1115.getConversion(rawVal, false)) 
        {
            task_error_data error_data = 
            {
                .timestamp = get_timestamp(),
                .task_id = TASK_ID_FORCE_SENSOR,
                .error_id = static_cast<uint32_t>(WARNING_FORCE_SENSOR_ADS1115_GET_CONVERSION_FAILURE)
            };
            _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
            osDelay(TASK_WARNING_RETRY_OSDELAY);
            continue; 
        }

        outputData.force = GetForce(rawVal);
        outputData.timestamp = get_timestamp();
        outputData.raw_value = rawVal;

        _data_buffer_writer.WriteElementAndIncrementIndex(outputData);

        osDelay(FORCESENSOR_TASK_OSDELAY);  // allow other tasks to run
    }
}




float ForceSensorADS1115::GetForce(uint16_t raw_value)
{
    // mv per count * count / 1000 to get volts / supply voltage to get ratio * max force in lbf * lbf to newton
    return _ads1115.getMvPerCount() * raw_value  / 1000 / ADS1115_VOLTAGE * MAX_FORCE_LBF * LBF_TO_NEWTON;
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
		 osThreadSuspend(osThreadGetId());;
	}

    forcesensor.Run();

}
