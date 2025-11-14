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
		void ToggleOPS(bool);
		friend void optical_sensor_interrupt(TIM_HandleTypeDef* htim);
		friend void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

		TIM_HandleTypeDef* _opticalTimer;
		osMessageQueueId_t _sessionControllerToOpticalSensorHandle;
		osMessageQueueId_t _opticalSensorToSessionControllerHandle;

		static OpticalSensor* _instance;
		optical_encoder_input_data _optical;
		bool _opEcdrEnabled;

};

OpticalSensor* OpticalSensor::_instance = nullptr; // ISR cannot call members directly, so we need an instance variable

OpticalSensor::OpticalSensor(TIM_HandleTypeDef* opticalTimer,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle) : /* Constructor */

		_opticalTimer(opticalTimer),
		_sessionControllerToOpticalSensorHandle(sessionControllerToOpticalSensorHandle),
		_opticalSensorToSessionControllerHandle(opticalSensorToSessionControllerHandle)
{
    // Link ISR to this instance
    _instance = this;

    // Initialize structure
    _optical.numOverflows = 0;
    _optical.IC_Value1 = 0;
    _optical.IC_Value2 = 0;
    _optical.timeDifference = 0;
}

bool OpticalSensor::Init()
{
	return true;
}

void OpticalSensor::Run(void)
{
	bool enableOPS = false;
	optical_encoder_output_data outputData; // Takes in TIMER data, not ADC

	while (1)
	{
		osMessageQueueGet(_sessionControllerToOpticalSensorHandle, &enableOPS, NULL, 0);
		if (enableOPS) {
			osMessageQueuePut(_opticalSensorToSessionControllerHandle, &outputData, 0, 0);
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

float OpticalSensor::GetRPM(uint16_t adcValue)
{
	return (_opticalTimer != 0) ? (float) ((CLK_SPEED / (_instance->_opticalTimer->Instance->PSC + 1)) * 60)/(_instance->_optical.timeDifference*NUM_APERTURES) : 0;
	// Need to see how to configure timeDifference
}


extern "C" void optical_sensor_interrupt(TIM_HandleTypeDef* htim)
{
    if (OpticalSensor::_instance == nullptr) return;

    OpticalSensor* os = OpticalSensor::_instance;

    // Read capture value
    os->_optical.IC_Value2 =
        HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    // Compute time difference (with overflow handling)
    os->_optical.timeDifference = (uint32_t)(os->_optical.IC_Value2 - os->_optical.IC_Value1) + (os->_optical.numOverflows * (htim->Instance->ARR + 1));
    os->_optical.IC_Value1 = os->_optical.IC_Value2;
    os->_optical.numOverflows = 0;
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
