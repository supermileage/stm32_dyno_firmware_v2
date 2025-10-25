#ifndef INC_XQUEUE_H_
#define INC_XQUEUE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdbool.h>

bool InitAllQueues();

extern QueueHandle_t sessionControllerToLumexLCDqHandle;

typedef enum
{
	NONE = 0,
	CLEAR_DISPLAY,
	WRITE_TO_DISPLAY,
} session_controller_to_lumex_lcd_opcode;

typedef enum
{
	START_PWM = 0,
	STOP_PWM,
	SET_DUTY_CYCLE,
} session_controller_to_bpm_opcode;

typedef struct {
	session_controller_to_lumex_lcd_opcode op;
    uint8_t row;
    uint8_t column;
    char* display_string;
} session_controller_to_lumex_lcd;


typedef struct {
	session_controller_to_bpm_opcode op;
	uint16_t new_duty_cycle; // only look  at this if it needs changing
} session_controller_to_bpm;

#endif /* INC_XQUEUE_H_ */
