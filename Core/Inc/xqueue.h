#ifndef INC_XQUEUE_H_
#define INC_XQUEUE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdbool.h>

bool InitAllQueues();

extern QueueHandle_t sessionControllerToLumexLCDqHandle;

/* Pertains to LCD */
typedef enum{
	NONE = 0,
	CLEAR_DISPLAY,
	WRITE_TO_DISPLAY,
} session_controller_to_lumex_lcd_opcode;

typedef struct {
	session_controller_to_lumex_lcd_opcode op;
    uint8_t row;
    uint8_t column;
    char* display_string;
} session_controller_to_lumex_lcd;


/* Pertains to brake power module */ 

typedef enum {
	START_PWM = 0,
	STOP_PWM,
	WRITE_NEW_DUTY_CYCLE
} session_controller_to_brake_pwm_opcode;

typedef struct {
	session_controller_to_brake_pwm_opcode op;
	uint16_t duty_cycle;
} session_controller_to_brake_pwm;

#endif /* INC_XQUEUE_H_ */
