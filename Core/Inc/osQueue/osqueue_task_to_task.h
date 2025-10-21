#ifndef INC_OSQUEUE_TASK_TO_TASK_H_
#define INC_OSQUEUE_TASK_TO_TASK_H_

#include <stdint.h>

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




#endif /* INC_OSQUEUE_TASK_TO_TASK_H_ */
