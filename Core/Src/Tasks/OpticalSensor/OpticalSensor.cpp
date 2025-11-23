#include <Tasks/OpticalSensor/OpticalSensor.hpp>
#include <Tasks/OpticalSensor/opticalsensor_main.h>

OpticalSensor* OpticalSensor::_instance = nullptr; // ISR cannot call members directly, so we need an instance variable

OpticalSensor::OpticalSensor(TIM_HandleTypeDef* opticalTimer,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle) : /* Constructor */
		// this comes directly from circular_buffers.h and config.h
		_buffer_writer(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
		_opticalTimer(opticalTimer),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle)
{
    // Link ISR to this instance
    _instance = this;

    // Initialize structure
    _optical.numOverflows = 0;
    _optical.IC_Value1 = 0;
    _optical.IC_Value2 = 0;
    _optical.timeDifference = 0;
    _optical.timestamp = 0;
}

bool OpticalSensor::Init()
{
	return true;
}

void OpticalSensor::Run(void)
{
	// Enable Optical Sensor bool
	bool enableOPS = false;

	// struct with the data which will be sent to the target modules
	optical_encoder_output_data outputData;

	while (1)
	{
		osMessageQueueGet(_sessionControllerToOpticalSensorHandle, &enableOPS, NULL, 0);
		if (enableOPS) {
			// Copies are made to prevent interrupts from overwriting data
			// Does not require a lock since making copies use one clock cycle
			uint32_t timestampCopy = _optical.timestamp;
			uint16_t timeDifferenceCopy = _optical.timeDifference;

			// populates the struct data
			outputData.timestamp = timestampCopy;
			outputData.rpm = GetRPM(timeDifferenceCopy);
			_buffer_writer.WriteElementAndIncrementIndex(outputData);
		}

	}
}

void OpticalSensor::ToggleOPS(bool enable)
{
	// if master enables and BPM was previously disabled, then start PWM
	if (enable && !_opEcdrEnabled)
	{
		HAL_TIM_PWM_Start(_opticalTimer, TIM_CHANNEL_1);
	}
	// if master enables and BPM was previously enabled, then stop PWM
	else if (!enable && _opEcdrEnabled)
	{
		HAL_TIM_PWM_Stop(_opticalTimer, TIM_CHANNEL_1);
	}

	_opEcdrEnabled = enable;
}

float OpticalSensor::GetRPM(uint16_t timeDifference)
{
	// Calculates the RPM based on the timeDifference between two rising edges
	return (timeDifference != 0) ? (float) ((CLK_SPEED / (_instance->_opticalTimer->Instance->PSC + 1)) * 60)/(timeDifference*NUM_APERTURES) : 0;
}

//uint32_t OpticalSensor::GetClockSpeed()
//{
//	uint32_t tim14_clk = HAL_RCC_GetPCLK1Freq();
//	/* If APB1 prescaler > 1, timer clock = PCLK1 * 2 */
//	if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1)
//	{
//	    tim14_clk *= 2;
//	}
//
//	return tim14_clk;
//}

void OpticalSensor::HandleCaptureInterrupt()
{
    _optical.IC_Value2 = HAL_TIM_ReadCapturedValue(_opticalTimer, TIM_CHANNEL_1);
    _optical.timestamp = get_timestamp();
    _optical.timeDifference = (uint32_t)(_optical.IC_Value2 - _optical.IC_Value1) +
                              (_optical.numOverflows * (_opticalTimer->Instance->ARR + 1));
    _optical.IC_Value1 = _optical.IC_Value2;
    _optical.numOverflows = 0;
}

void OpticalSensor::HandleOverflowInterrupt()
{
    if (_optical.numOverflows != OP_OF) {
        _optical.numOverflows++;
    } else {
        _optical.timeDifference = 0;
    }
}



// ISR wrappers
extern "C" void optical_sensor_output_interrupt()
{
    if (OpticalSensor::_instance) OpticalSensor::_instance->HandleCaptureInterrupt();
}

extern "C" void optical_sensor_overflow_interrupt()
{
    if (OpticalSensor::_instance) OpticalSensor::_instance->HandleOverflowInterrupt();
}

extern "C" void optical_sensor_main(TIM_HandleTypeDef* opticalTimer, osMessageQueueId_t sessionControllerToOpticalSensorHandle)
{
	OpticalSensor opticalsensor = OpticalSensor(opticalTimer, sessionControllerToOpticalSensorHandle);

	if (!opticalsensor.Init())
	{
		return;
	}

	while(1)
	{
		opticalsensor.Run();
	}
}
