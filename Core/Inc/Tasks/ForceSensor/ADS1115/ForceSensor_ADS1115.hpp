#ifndef INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_
#define INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_

#include "main.h"
#include "cmsis_os.h"

#include "timekeeping/timestamps.h"

#include "MessagePassing/circular_buffers.h"
#include "CircularBufferWriter.hpp"

#include "ADS1115.hpp"

class ForceSensorADS1115 
{
	public:
		ForceSensorADS1115(I2C_HandleTypeDef* i2cHandle,
				osMessageQueueId_t sessionControllerToForceSensorHandle);
		virtual ~ForceSensorADS1115() = default;

		bool Init();
		void Run();

	private:
		float GetForce(void);

		// Circular Buffer for ForceSensor with template bpm_output_data
		CircularBufferWriter<forcesensor_output_data> _buffer_writer;

		ADS1115 _ads1115;

		I2C_HandleTypeDef* _i2cHandle;

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;

		

};


#endif /* INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_ */
