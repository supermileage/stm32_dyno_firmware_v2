#include <opticalsensor/opticalsensor.h>

#define CLK_SPEED 64000000 // Hard coded values for the GetRPM function
#define NUM_APERTURES 64

volatile uint32_t timestamp_os = 0;
volatile float rpm = 0;

class OpticalSensor /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		OpticalSensor(TIM_HandleTypeDef* opticalTimer,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle);

		virtual ~OpticalSensor() = default;

		bool Init();
		void Run();

		typedef struct
				{
					uint8_t numOverflows;
					uint16_t IC_Value1;
					uint16_t IC_Value2;
					uint32_t timeDifference;

				} optical_encoder_input_data;

	private:
		float GetRPM(uint16_t);
		friend void optical_sensor_interrupt(TIM_HandleTypeDef* htim);
		friend void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

		TIM_HandleTypeDef* _opticalTimer;
		osMessageQueueId_t _sessionControllerToOpticalSensorHandle;
		osMessageQueueId_t _opticalSensorToSessionControllerHandle;

		static OpticalSensor* instance;
		optical_encoder_input_data optical;

};

OpticalSensor* OpticalSensor::instance = nullptr; // ISR cannot call members directly, so we need an instance variable

OpticalSensor::OpticalSensor(TIM_HandleTypeDef* opticalTimer,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle) : /* Constructor */

		_opticalTimer(opticalTimer),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_opticalSensorToSessionControllerHandle(opticalSensorToSessionControllerHandle)
{
    // Link ISR to this instance
    instance = this;

    // Initialize structure
    optical.numOverflows = 0;
    optical.IC_Value1 = 0;
    optical.IC_Value2 = 0;
    optical.timeDifference = 0;
}

bool OpticalSensor::Init()
{
	return true;
}

void OpticalSensor::Run(void)
{
	optical_encoder_output_data outputData; // Takes in TIMER data, not ADC

	while (1)
	{
		osMessageQueuePut(_opticalSensorToSessionControllerHandle, &outputData, 0, 0);

	}
}

float OpticalSensor::GetRPM(uint16_t adcValue)
{
	return (_opticalTimer != 0) ? (float) ((CLK_SPEED / (instance->_opticalTimer->Instance->PSC + 1)) * 60)/(instance->optical.timeDifference*NUM_APERTURES) : 0;
	// Need to see how to configure timeDifference
}


extern "C" void optical_sensor_interrupt(TIM_HandleTypeDef* htim)
{
    if (OpticalSensor::instance == nullptr) return;

    OpticalSensor* os = OpticalSensor::instance;

    // Read capture value
    os->optical.IC_Value2 =
        HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    // Compute time difference (with overflow handling)
    os->optical.timeDifference =
        (uint32_t)(os->optical.IC_Value2 - os->optical.IC_Value1)
        + (os->optical.numOverflows * (htim->Instance->ARR + 1));

    os->optical.IC_Value1 = os->optical.IC_Value2;
    os->optical.numOverflows = 0;
}

extern "C" void optical_sensor_main(TIM_HandleTypeDef* opticalTimer, osMessageQueueId_t sessionControllerToOpticalSensorHandle, osMessageQueueId_t opticalSensorToSessionControllerHandle)
{
	OpticalSensor opticalsensor = OpticalSensor(opticalTimer, sessionControllerToOpticalSensorHandle, opticalSensorToSessionControllerHandle);

	if (!opticalsensor.Init())
	{
		return;
	}

	while(1)
	{
		opticalsensor.Run();
	}
}
