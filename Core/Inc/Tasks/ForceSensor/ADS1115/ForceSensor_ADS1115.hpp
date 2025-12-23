#ifndef INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_
#define INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_

#include "main.h"
#include "cmsis_os.h"

#include "Config/hal_instances.h"

#include "Timekeeping/timestamps.h"

#include "MessagePassing/circular_buffers.h"
#include "CircularBufferWriter.hpp"

#include "ADS1115.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class ForceSensorADS1115 
{
	public:
		ForceSensorADS1115(osMessageQueueId_t sessionControllerToForceSensorHandle);
		~ForceSensorADS1115() = default;

		bool Init();
		void Run();

	private:
		float GetForce(uint16_t rawValue);

		// Circular Buffer for ForceSensor with template bpm_output_data
		CircularBufferWriter<forcesensor_output_data> _data_buffer_writer;
		CircularBufferWriter<task_errors> _task_error_buffer_writer;

		ADS1115 _ads1115;

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;

		

};

#ifdef __cplusplus
}
#endif




#endif /* INC_FORCESENSOR_ADS1115_FORCESENSOR_ADS1115_HPP_ */
