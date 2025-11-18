#ifndef INC_OPTICALSENSOR_HPP_
#define INC_OPTICALSENSOR_HPP_

#include <string.h>

class OpticalSensor /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		OpticalSensor(TIM_HandleTypeDef* opticalTimer,
				TIM_HandleTypeDef* timestampTimer,
				osMessageQueueId_t sessionControllerToOpticalSensorHandle,
				osMessageQueueId_t opticalSensorToSessionControllerHandle);

		virtual ~OpticalSensor() = default;

		bool Init();
		void Run();

		typedef struct
		{
			uint32_t numOverflows;
			uint32_t numInterruptCbs;
			uint16_t IC_Value1;
			uint16_t IC_Value2;
			uint32_t timeDifference;
			uint32_t timestamp_os = 0;

		} optical_encoder_input_data;

		TIM_HandleTypeDef* _opticalTimer;
		TIM_HandleTypeDef* _timestampTimer;
		
		volatile static optical_encoder_input_data _optical;
		static OpticalSensor* _instance;

	private:
		float GetRPM(uint16_t);
//		uint32_t GetClockSpeed();
		void ToggleOPS(bool);

		
		osMessageQueueId_t _sessionControllerToOpticalSensorHandle;
		osMessageQueueId_t _opticalSensorToSessionControllerHandle;

		bool _opEcdrEnabled;
		uint32_t _localNumInterruptCbs;

		
		
		
//		const uint32_t _clock_speed;
};

#endif //
