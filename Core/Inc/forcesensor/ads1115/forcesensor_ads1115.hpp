#ifndef INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_
#define INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_

#include "main.h"
#include "cmsis_os.h"

#include "ADS1115.hpp"
#include "ADS1115_main.h"

class ForcesensorADS1115 /* Class definition because we can't use headers for C++ based on this implementation method */
{
	public:
		ForcesensorADS1115(I2C_HandleTypeDef* i2cHandle,
                TIM_HandleTypeDef* timestampTimer,
				osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle);
		virtual ~ForcesensorADS1115() = default;

		bool Init();
		void Run();

	private:
		float GetForce(void);

		I2C_HandleTypeDef* _i2cHandle;
        TIM_HandleTypeDef* _timer;

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;

		ADS1115 _ads1115;

};


#endif /* INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_ */
