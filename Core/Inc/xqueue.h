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

typedef struct {
	session_controller_to_lumex_lcd_opcode op;
    uint8_t row;
    uint8_t column;
    char* display_string;
} session_controller_to_lumex_lcd;




#endif /* INC_XQUEUE_H_ */
