#ifndef INC_FORCESENSOR_FORCESENSOR_ADC_HPP_
#define INC_FORCESENSOR_FORCESENSOR_ADC_HPP_

#include "main.h"
#include "cmsis_os2.h"

#include "osQueue/osqueue_task_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

class ForcesensorADC
{
	public:
		ForcesensorADC(ADC_HandleTypeDef* adcHandle,
				osMessageQueueId_t sessionControllerToForceSensorHandle,
				osMessageQueueId_t forceSensorToSessionControllerHandle);
		virtual ~ForcesensorADC() = default;

		bool Init();
		void Run();

	private:
		float GetForce(uint16_t adcValue);

		ADC_HandleTypeDef* _adcHandle;

		osMessageQueueId_t _sessionControllerToForceSensorHandle;
		osMessageQueueId_t _forceSensorToSessionControllerHandle;

};

#ifdef __cplusplus
}
#endif

#endif /* INC_FORCESENSOR_FORCESENSOR_ADC_HPP_ */
