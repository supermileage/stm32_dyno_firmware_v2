#ifndef INC_LCD_LUMEXLCD_MAIN_H_
#define INC_LCD_LUMEXLCD_MAIN_H_

#include "main.h"

#include "cmsis_os2.h"


#ifdef __cplusplus
extern "C" {
#endif

void lumex_lcd_timer_interrupt(TIM_HandleTypeDef* timer);
void lumex_lcd_main(TIM_HandleTypeDef* timer, osMessageQueueId_t lumexLcdToSessionControllerqHandle);

#ifdef __cplusplus
}
#endif




#endif /* INC_LCD_LUMEXLCD_MAIN_H_ */
