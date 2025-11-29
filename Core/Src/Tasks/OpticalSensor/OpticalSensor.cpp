#include <Tasks/OpticalSensor/OpticalSensor.hpp>
#include <Tasks/OpticalSensor/opticalsensor_main.h>


static volatile uint32_t numOverflows = 0;
static volatile uint32_t timestamp = 0;
static volatile uint16_t IC_Value1 = 0;
static volatile uint16_t IC_Value2 = 0;
static volatile uint16_t timeDifference = 0;

OpticalSensor::OpticalSensor(osMessageQueueId_t sessionControllerToOpticalSensorHandle) : 
		// this comes directly from circular_buffers.h and config.h
		_buffer_writer(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_opticalEncoderEnabled(false)
{}

bool OpticalSensor::Init()
{
	return true;
}

void OpticalSensor::Run(void)
{

	// struct with the data which will be sent to the target modules
	optical_encoder_output_data outputData;

	while (1)
	{
		osMessageQueueGet(_sessionControllerToOpticalSensorHandle, &_opticalEncoderEnabled, NULL, 0);
		if (_opticalEncoderEnabled) {
			// Copies are made to prevent interrupts from overwriting data
			// Does not require a lock since making copies use one clock cycle
			uint32_t timestampCopy = timestamp;
			uint16_t timeDifferenceCopy = timeDifference;

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
	if (enable && !_opticalEncoderEnabled)
	{
		HAL_TIM_PWM_Start(opticalTimer, TIM_CHANNEL_1);
	}
	// if master enables and BPM was previously enabled, then stop PWM
	else if (!enable && _opticalEncoderEnabled)
	{
		HAL_TIM_PWM_Stop(opticalTimer, TIM_CHANNEL_1);
	}

	_opticalEncoderEnabled = enable;
}

float OpticalSensor::GetRPM(uint16_t timeDifference)
{
	// Calculates the RPM based on the timeDifference between two rising edges
	return (timeDifference != 0) ? (float) ((CLK_SPEED / (opticalTimer->Instance->PSC + 1)) * 60)/(timeDifference*NUM_APERTURES) : 0;
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

extern "C" void opticalsensor_output_interrupt()
{
    IC_Value2 = HAL_TIM_ReadCapturedValue(opticalTimer, TIM_CHANNEL_1);
    timestamp = get_timestamp();
    timeDifference = (uint32_t)(IC_Value2 - IC_Value1) +
                              (numOverflows * (opticalTimer->Instance->ARR + 1));
    IC_Value1 = IC_Value2;
    numOverflows = 0;
}

extern "C" void opticalsensor_overflow_interrupt()
{
    if (numOverflows != OP_OF) {
        numOverflows++;
    } else {
        timeDifference = 0;
    }
}

extern "C" void opticalsensor_main(osMessageQueueId_t sessionControllerToOpticalSensorHandle)
{
	OpticalSensor opticalsensor = OpticalSensor(sessionControllerToOpticalSensorHandle);

	if (!opticalsensor.Init())
	{
		return;
	}

	while(1)
	{
		opticalsensor.Run();
	}
}
