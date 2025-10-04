#ifndef INC_LCD_LUMEXLCD_H_
#define INC_LCD_LUMEXLCD_H_

#include "main.h"
#include "string.h"

#include "xqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

void lumex_lcd_main(TIM_HandleTypeDef* timer, QueueHandle_t qHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_LCD_LUMEXLCD_H_ */
