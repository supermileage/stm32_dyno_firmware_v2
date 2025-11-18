#ifndef INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_
#define INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_

#include "main.h"
#include "cmsis_os.h"

#include "timekeeping/timestamps.h"

#include "MessagePassing/circular_buffers.h"
#include "CircularBufferWriter.hpp"

class ForceSensorADS1115 /* Class definition because we can't use headers for C++ based on this implementation method */
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

		I2C_HandleTypeDef* _i2cHandle;

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;

};


#endif /* INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_ */
