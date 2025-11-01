#ifndef INC_OSQUEUE_OSQUEUE_INTERRUPT_TO_TASK_H_
#define INC_OSQUEUE_OSQUEUE_INTERRUPT_TO_TASK_H_

#include "main.h"

typedef struct {
	uint32_t timestamp;
	uint16_t adc_value;
} adc_callback_to_forcesensor;

#endif /* INC_OSQUEUE_OSQUEUE_INTERRUPT_TO_TASK_H_ */
