#ifndef INC_OPTICALSENSOR_HPP_
#define INC_OPTICALSENSOR_HPP_

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
					volatile uint32_t numOverflows;
					volatile uint16_t IC_Value1;
					volatile uint16_t IC_Value2;
					volatile uint32_t timeDifference;
					volatile uint32_t timestamp_os = 0;

				} optical_encoder_input_data;

	private:
		float GetRPM(uint16_t);
//		uint32_t GetClockSpeed();
		void ToggleOPS(bool);
		friend void optical_sensor_interrupt();
		friend void optical_sensor_overflow_interrupt();

		TIM_HandleTypeDef* _opticalTimer;
		TIM_HandleTypeDef* _timestampTimer;
		osMessageQueueId_t _sessionControllerToOpticalSensorHandle;
		osMessageQueueId_t _opticalSensorToSessionControllerHandle;

		static OpticalSensor* _instance;
		optical_encoder_input_data _optical;
		bool _opEcdrEnabled;
//		const uint32_t _clock_speed;
};

#endif //
