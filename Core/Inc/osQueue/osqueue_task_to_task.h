#ifndef INC_OSQUEUE_TASK_TO_TASK_H_
#define INC_OSQUEUE_TASK_TO_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_os.h"


typedef enum
{
	CLEAR_DISPLAY = 0,
	WRITE_TO_DISPLAY = 1,
} session_controller_to_lumex_lcd_opcode;

typedef struct {
	session_controller_to_lumex_lcd_opcode op;
    uint8_t row;
    uint8_t column;
    char* display_string;
} session_controller_to_lumex_lcd;

typedef enum
{
	START_PWM = 0,
	STOP_PWM,
	SET_DUTY_CYCLE,
} session_controller_to_bpm_opcode;

typedef struct {
	session_controller_to_bpm_opcode op;
	uint16_t new_duty_cycle; // only look  at this if it needs changing
} session_controller_to_bpm;









#endif /* INC_OSQUEUE_TASK_TO_TASK_H_ */
