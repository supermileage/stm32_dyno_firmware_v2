#include <Tasks/OpticalSensor/OpticalSensor.hpp>
#include <Tasks/OpticalSensor/opticalsensor_main.h>

extern TIM_HandleTypeDef* opticalTimer;

extern size_t optical_encoder_circular_buffer_index_writer;
extern optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];

extern size_t task_error_circular_buffer_index_writer;
extern task_error_data task_error_circular_buffer[TASK_ERROR_CIRCULAR_BUFFER_SIZE];

static volatile bool new_data = false;
static volatile uint32_t numOverflows = 0;
static volatile uint32_t timestamp = 0;
static volatile uint16_t IC_Value1 = 0;
static volatile uint16_t IC_Value2 = 0;

// timerCounterDifference is the delta of the timer counter between IC_Value2 and IC_Value1
static volatile uint32_t timerCounterDifference = 0;
static volatile uint32_t prevTimerCounterDifference = 0;

OpticalSensor::OpticalSensor(osMessageQueueId_t sessionControllerToOpticalSensorHandle) : 
		// this comes directly from circular_buffers.h and config.h
		_data_buffer_writer(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_clockSpeed(get_timer_clock(opticalTimer->Instance)),
		_opticalEncoderEnabled(false)
{}

bool OpticalSensor::Init()
{
	HAL_TIM_IC_Start_IT(opticalTimer, TIM_CHANNEL_1);
    return true;
}

void OpticalSensor::Run(void)
{
    optical_encoder_output_data outputData;

    while (1)
    {
        osDelay(OPTICAL_ENCODER_TASK_OSDELAY);
        // --- Get the latest enable/disable state ---
        GetLatestFromQueue(
            _sessionControllerToOpticalSensorHandle,
            &_opticalEncoderEnabled,
            sizeof(_opticalEncoderEnabled),
            _opticalEncoderEnabled ? 0 : osWaitForever
        );

        // Skip processing if the latest state says disabled
        if (!_opticalEncoderEnabled)
        {
        	continue;
        }

        // --- Copy critical data to avoid race conditions ---
        taskENTER_CRITICAL();
        if (!new_data)
		{
        	taskEXIT_CRITICAL();
        	continue;
		}
        uint32_t timestampCopy = timestamp;
        uint32_t timerCounterDifferenceCopy = timerCounterDifference;
        uint32_t prevTimerCounterDifferenceCopy = prevTimerCounterDifference;
        new_data = false;
        taskEXIT_CRITICAL();

        // --- Populate output struct ---
        outputData.timestamp = timestampCopy;
        outputData.angular_velocity = CalculateAngularVelocity(timerCounterDifferenceCopy);
        outputData.angular_acceleration = CalculateAngularAcceleration(
            timerCounterDifferenceCopy,
            prevTimerCounterDifferenceCopy
        );

        _data_buffer_writer.WriteElementAndIncrementIndex(outputData);

    }
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
    uint32_t newDiff;

    IC_Value2 = HAL_TIM_ReadCapturedValue(opticalTimer, TIM_CHANNEL_1);
    timestamp = get_timestamp();

    newDiff = (uint32_t)(IC_Value2 - IC_Value1) +
              (numOverflows * (opticalTimer->Instance->ARR + 1));

    IC_Value1 = IC_Value2;
    numOverflows = 0;

    prevTimerCounterDifference = timerCounterDifference;
    timerCounterDifference = newDiff;

    new_data = true;
}

syntax error

extern "C" void opticalsensor_overflow_interrupt()
{
    if (numOverflows != OPTICAL_MAX_NUM_OVERFLOWS) {
        numOverflows = numOverflows + 1;
    } else {
        timerCounterDifference = 0;
    }
}

extern "C" void opticalsensor_main(osMessageQueueId_t sessionControllerToOpticalSensorHandle)
{
	OpticalSensor opticalsensor = OpticalSensor(sessionControllerToOpticalSensorHandle);

	if (!opticalsensor.Init())
	{
		 osThreadSuspend(osThreadGetId());;
	}


    opticalsensor.Run();

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

