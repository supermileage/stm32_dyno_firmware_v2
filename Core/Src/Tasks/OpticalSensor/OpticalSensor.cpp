#include <Tasks/OpticalSensor/OpticalSensor.hpp>
#include <Tasks/OpticalSensor/opticalsensor_main.h>

static volatile uint32_t numOverflows = 0;
static volatile uint32_t timestamp = 0;
static volatile uint16_t IC_Value1 = 0;
static volatile uint16_t IC_Value2 = 0;

// timerCounterDifference is the delta of the timer counter between IC_Value2 and IC_Value1
static volatile uint32_t timerCounterDifference = 0;
static volatile uint32_t prevTimerCounterDifference = 0;

OpticalSensor::OpticalSensor(osMessageQueueId_t sessionControllerToOpticalSensorHandle) : 
		// this comes directly from circular_buffers.h and config.h
		_buffer_writer(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_clockSpeed(get_timer_clock(opticalTimer->Instance)),
		_opticalEncoderEnabled(false)
{}

bool OpticalSensor::Init()
{
	return true;
}

void OpticalSensor::Run(void)
{

    while (1)
	{
		bool previousState = _opticalEncoderEnabled;  // save current state

		// --- Get the latest enable/disable state ---
		GetLatestFromQueue(
			_sessionControllerToOpticalSensorHandle,
			&_opticalEncoderEnabled,
			sizeof(_opticalEncoderEnabled),
			_opticalEncoderEnabled ? 0 : osWaitForever
		);

		// If state changed, toggle encoder
		if (previousState != _opticalEncoderEnabled)
		{
			ToggleOpticalEncoder(_opticalEncoderEnabled);
		}

		// Skip processing if disabled
		if (!_opticalEncoderEnabled)
		{
			continue;
		}

		// --- Copy critical data to avoid race conditions ---
		taskENTER_CRITICAL();
		uint32_t timestampCopy = timestamp;
		uint32_t timerCounterDifferenceCopy = timerCounterDifference;
		uint32_t prevTimerCounterDifferenceCopy = prevTimerCounterDifference;
		taskEXIT_CRITICAL();

		// --- Populate output struct ---
		optical_encoder_output_data outputData;
		outputData.timestamp = timestampCopy;
		outputData.angular_velocity = CalculateAngularVelocity(timerCounterDifferenceCopy);
		outputData.angular_acceleration = CalculateAngularAcceleration(
			timerCounterDifferenceCopy,
			prevTimerCounterDifferenceCopy
		);

		_buffer_writer.WriteElementAndIncrementIndex(outputData);

		// --- Yield ---
		osDelay(OPTICAL_ENCODER_TASK_OSDELAY);
	}

}


void OpticalSensor::ToggleOpticalEncoder(bool enable)
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

float OpticalSensor::CalculateAngularAcceleration(uint32_t timerCounterDifference, uint32_t prevTimerCounterDifference)
{
    if (timerCounterDifference == 0 || prevTimerCounterDifference == 0) return 0;

    float omega_curr = CalculateAngularVelocity(timerCounterDifference);
    float omega_prev = CalculateAngularVelocity(prevTimerCounterDifference);

    // Δt in seconds between these two measurements
    float dt_avg = ((float)timerCounterDifference + (float)prevTimerCounterDifference) / 2.0f / (_clockSpeed / (opticalTimer->Instance->PSC + 1));

    float alpha = (omega_curr - omega_prev) / dt_avg;
    return alpha; // rad/s²
}


float OpticalSensor::CalculateAngularVelocity(uint32_t timerCounterDifference)
{
    if (timerCounterDifference == 0) return 0;

    // dt is timer ticks, convert to seconds
    float timerFreq = static_cast<float>(_clockSpeed) / (opticalTimer->Instance->PSC + 1);
    
	float omega = (2.0f * M_PI / NUM_APERTURES) * (timerFreq / timerCounterDifference); // radians/sec
    
	return omega;
}


float OpticalSensor::CalculateRPM(uint32_t timerCounterDifference)
{
    // Return 0 immediately if no pulse was detected
    if (timerCounterDifference == 0) {
        return 0.0f;
    }

    // Timer frequency in Hz (ticks per second)
    float timerFreq = static_cast<float>(_clockSpeed) / (opticalTimer->Instance->PSC + 1);

    // Time between pulses in seconds
    float deltaTimeSec = (float)timerCounterDifference / timerFreq;

    // RPM = revolutions per minute
    float rpm = ((1.0f / NUM_APERTURES) / deltaTimeSec) * 60.0f;

    return rpm;
}

extern "C" void opticalsensor_output_interrupt()
{
    IC_Value2 = HAL_TIM_ReadCapturedValue(opticalTimer, TIM_CHANNEL_1);
    timestamp = get_timestamp();
    timerCounterDifference = (uint32_t)(IC_Value2 - IC_Value1) +
                              (numOverflows * (opticalTimer->Instance->ARR + 1));
    IC_Value1 = IC_Value2;

	prevTimerCounterDifference = timerCounterDifference;
    numOverflows = 0;
}

extern "C" void opticalsensor_overflow_interrupt()
{
    if (numOverflows != OP_OF) {
        numOverflows++;
    } else {
        timerCounterDifference = 0;
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

