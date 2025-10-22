#ifndef INC_LCD_LUMEXLCD_H_
#define INC_LCD_LUMEXLCD_H_

#include "main.h"
#include "string.h"

#include "cmsis_os.h"

#include "osQueue/osqueue_task_to_task.h"
#include "osQueue/osqueue_interrupt_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

void lumex_lcd_timer_interrupt(TIM_HandleTypeDef* timer, osMessageQueueId_t timInterruptCallbackqHandle);
void lumex_lcd_main(TIM_HandleTypeDef* timer, osMessageQueueId_t lumexLcdToSessionControllerqHandle, osMessageQueueId_t timInterruptCallbackqHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_LCD_LUMEXLCD_H_ */
