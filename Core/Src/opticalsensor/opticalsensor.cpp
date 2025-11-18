#include <opticalsensor/opticalsensor_main.h>
#include <opticalsensor/opticalsensor.hpp>

OpticalSensor* OpticalSensor::_instance = nullptr; // ISR cannot call members directly, so we need an instance variable
volatile OpticalSensor::optical_encoder_input_data OpticalSensor::_optical = {};



OpticalSensor::OpticalSensor(TIM_HandleTypeDef* opticalTimer,
				TIM_HandleTypeDef* timestampTimer,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle
				) : /* Constructor */
		_opticalTimer(opticalTimer),
		_timestampTimer(timestampTimer),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_opticalSensorToSessionControllerHandle(opticalSensorToSessionControllerHandle), 
		_opEcdrEnabled(false),
		_localNumInterruptCbs(0)
{
    // Link ISR to this instance
    _instance = this;

    // Initialize structure
	_optical.numOverflows     = 0;
	_optical.numInterruptCbs  = 0;
	_optical.IC_Value1        = 0;
	_optical.IC_Value2        = 0;
	_optical.timeDifference   = 0;
	_optical.timestamp_os     = 0;
}

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
		osMessageQueueGet(_sessionControllerToOpticalSensorHandle, &_opEcdrEnabled, NULL, 0);

		uint32_t copyNumInterruptCbs = _optical.numInterruptCbs;
		if (_opEcdrEnabled && copyNumInterruptCbs != _localNumInterruptCbs) {
			// Copies are made to prevent interrupts from overwriting data
			// Does not require a lock since making copies use one clock cycle
			uint32_t timestampCopy = _optical.timestamp_os;
			uint32_t timeDifferenceCopy = _optical.timeDifference;

			// populates the struct data
			outputData.timestamp_os = timeDifferenceCopy;
			outputData.rpm = GetRPM(timestampCopy);
			osMessageQueuePut(_opticalSensorToSessionControllerHandle, &outputData, 0, 0);

			_localNumInterruptCbs = copyNumInterruptCbs;
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
	return (timeDifference != 0) ? (float) ((CLK_SPEED / (_opticalTimer->Instance->PSC + 1)) * 60)/(timeDifference*NUM_APERTURES) : 0;
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


extern "C" void optical_sensor_output_interrupt()
{
    if (OpticalSensor::_instance == nullptr) return;

    OpticalSensor* os = OpticalSensor::_instance;

    // Read capture value from the optical timer
    os->_optical.IC_Value2 = HAL_TIM_ReadCapturedValue(os->_opticalTimer, TIM_CHANNEL_1);

    // gets the timestamp value
	os->_optical.timestamp_os = __HAL_TIM_GET_COUNTER(os->_timestampTimer);

    // Compute time difference (with overflow handling)
    os->_optical.timeDifference = (uint32_t)(os->_optical.IC_Value2 - os->_optical.IC_Value1) + (os->_optical.numOverflows * (os->_opticalTimer->Instance->ARR + 1));
    os->_optical.IC_Value1 = os->_optical.IC_Value2;

    // restarting the number of timer overflows. This is used to check whether the optical encoder has stopped spinning
    os->_optical.numOverflows = 0;

	os->_optical.numInterruptCbs++;


}

// This function gets called once the timer counter overflows.
extern "C" void optical_sensor_overflow_interrupt()
{
	if (OpticalSensor::_instance == nullptr) return;

	OpticalSensor* os = OpticalSensor::_instance;

	// Every time the timer overflows, increment the value
	if (os->_optical.numOverflows != OP_OF) {
		os->_optical.numOverflows++;
	}
	else
	{
		// in GetRPM, if timeDifference is 0, then associate that with 0 speed.
		os->_optical.timeDifference = 0;
	}
}

extern "C" void optical_sensor_main(TIM_HandleTypeDef* opticalTimer, TIM_HandleTypeDef* timestampTimer, osMessageQueueId_t sessionControllerToOpticalSensorHandle, osMessageQueueId_t opticalSensorToSessionControllerHandle)
{
	OpticalSensor opticalsensor = OpticalSensor(opticalTimer, timestampTimer, sessionControllerToOpticalSensorHandle, opticalSensorToSessionControllerHandle);

	if (!opticalsensor.Init())
	{
		return;
	}

	while(1)
	{
		opticalsensor.Run();
	}
}
