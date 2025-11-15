#ifndef INC_LCD_LUMEXLCD_HPP_
#define INC_LCD_LUMEXLCD_HPP_

#include "main.h"

#include "cmsis_os.h"

#include "string.h"

#include "osQueue/osqueue_task_to_task.h"

#ifdef __cplusplus
extern "C" {
#endif

class LumexLCD
{
	public:
		LumexLCD(TIM_HandleTypeDef* timer, osMessageQueueId_t lumexLcdToSessionControllerqHandle, osMessageQueueId_t timInterruptCallbackqHandle);
		virtual ~LumexLCD() = default;

		bool Init();
		void Run();


	private:
		bool StartTimer(uint8_t microseconds);
		bool SendByte(uint8_t byte);
		bool WriteData(uint8_t data);
		bool WriteCommand(uint8_t command);
		bool ClearDisplay();
		bool SetCursor(uint8_t row, uint8_t column);
		bool DisplayChar(uint8_t row, uint8_t column, uint8_t character);
		bool DisplayString(uint8_t row, uint8_t column, char* string);

		TIM_HandleTypeDef* _timer;
		osMessageQueueId_t _fromSCqHandle;
		osMessageQueueId_t _timqHandle;
		volatile bool _timerflag;
};


#ifdef __cplusplus
}
#endif



#endif /* INC_LCD_LUMEXLCD_HPP_ */
