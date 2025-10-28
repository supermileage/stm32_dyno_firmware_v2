#include <lcd/lumexLcd.h>

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

		session_controller_to_lumex_lcd _msg;
};

LumexLCD::LumexLCD(TIM_HandleTypeDef* timer, osMessageQueueId_t lumexLcdToSessionControllerqHandle, osMessageQueueId_t timInterruptCallbackqHandle) :
		_timer(timer),
		_fromSCqHandle(lumexLcdToSessionControllerqHandle),
		_timqHandle(timInterruptCallbackqHandle),
		_msg{}
{}

bool LumexLCD::Init()
{

    // Enable to GND to tell that we are in command mode, not data mode
	HAL_GPIO_WritePin(LUMEX_LCD_EN_GPIO_Port, LUMEX_LCD_EN_Pin, GPIO_PIN_RESET);

    HAL_Delay(40);


    // Proper 8-bit mode initialization sequence
    // Function set: 8-bit mode, 2-line, 5x8 font
    if (!WriteCommand(0x38))
    {
    	return false;
    }

    HAL_Delay(5);


    // needs to be done twice
    if (!WriteCommand(0x38))
	{
		return false;
	}

	HAL_Delay(5);

    // just to make sure it works
	if (!WriteCommand(0x38))
	{
		return false;
	}

	HAL_Delay(5);

    // Display ON, Cursor OFF, Blink OFF
	if (!WriteCommand(0x0c))
	{
		return false;
	}

	HAL_Delay(5);

    // Clear Display
    if (!ClearDisplay())
	{
    	return false;
	}

    return true;
}

void LumexLCD::Run(void)
{
	osStatus_t status;
	while(1)
	{

		status = osMessageQueueGet(_fromSCqHandle, &_msg, 0, 0);

		if (status != osOK)
		 continue;

		switch(_msg.op)
		{
			case CLEAR_DISPLAY:
				ClearDisplay();
				break;
			case WRITE_TO_DISPLAY:
				DisplayString(_msg.row, _msg.column, _msg.display_string);
				break;
			default:
				break;

		}
	}
}



bool LumexLCD::StartTimer(uint8_t microseconds)
{
	__HAL_TIM_SET_COUNTER(_timer, 0);
	__HAL_TIM_SET_AUTORELOAD(_timer, microseconds);
	if (HAL_TIM_Base_Start_IT(_timer) != HAL_OK)
	{
		return false;
	}

	return true;
}



bool LumexLCD::SendByte(uint8_t byte)
{
	// Very Inefficient Way of Toggling Pins but may decide to use registers instead in the future
	HAL_GPIO_WritePin(LUMEX_LCD_D7_GPIO_Port, LUMEX_LCD_D7_Pin, static_cast<GPIO_PinState>((byte >> 7) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D6_GPIO_Port, LUMEX_LCD_D6_Pin, static_cast<GPIO_PinState>((byte >> 6) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D5_GPIO_Port, LUMEX_LCD_D5_Pin, static_cast<GPIO_PinState>((byte >> 5) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D4_GPIO_Port, LUMEX_LCD_D4_Pin, static_cast<GPIO_PinState>((byte >> 4) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D3_GPIO_Port, LUMEX_LCD_D3_Pin, static_cast<GPIO_PinState>((byte >> 3) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D2_GPIO_Port, LUMEX_LCD_D2_Pin, static_cast<GPIO_PinState>((byte >> 2) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D1_GPIO_Port, LUMEX_LCD_D1_Pin, static_cast<GPIO_PinState>((byte >> 1) & 0x01));
	HAL_GPIO_WritePin(LUMEX_LCD_D0_GPIO_Port, LUMEX_LCD_D0_Pin, static_cast<GPIO_PinState>((byte >> 0) & 0x01));


	// Set EN Pin and start timer
	HAL_GPIO_WritePin(LUMEX_LCD_EN_GPIO_Port, LUMEX_LCD_EN_Pin, GPIO_PIN_SET);

	if (!StartTimer(40))
	{
		return false;
	}

	HAL_StatusTypeDef status;
	osMessageQueueGet(_timqHandle, &status, NULL, osWaitForever);
	if (status != HAL_OK)
	{
		return false;
	}

	return true;

}

bool LumexLCD::WriteData(uint8_t data)
{
	HAL_GPIO_WritePin(LUMEX_LCD_RS_GPIO_Port, LUMEX_LCD_RS_Pin, GPIO_PIN_SET);

	if (!SendByte(data))
	{
		return false;
	}
	return true;

}


bool LumexLCD::WriteCommand(uint8_t command)
{
	HAL_GPIO_WritePin(LUMEX_LCD_RS_GPIO_Port, LUMEX_LCD_RS_Pin, GPIO_PIN_RESET);

	if (!SendByte(command))
	{
		return false;
	}

	return true;
}

bool LumexLCD::ClearDisplay()
{
	if (!WriteCommand(0x01))
	{
		return false;
	}

	HAL_Delay(20);

	return true;
}


bool LumexLCD::SetCursor(uint8_t row, uint8_t column) {

	uint8_t address = (row == 0) ? 0x00 : 0x40;
	address += column;
	if (!WriteCommand(0x80 | address))
	{
		return false;
	}

	return true;

}

bool LumexLCD::DisplayChar(uint8_t row, uint8_t column, uint8_t character)
{
	if (!SetCursor(row, column))
	{
		return false;
	}

	if (!WriteData(character))
	{
		return false;
	}

	return true;
}

bool LumexLCD::DisplayString(uint8_t row, uint8_t column, char* string)
{
	for (uint8_t i = 0; i < strlen(string); i++)
	{
		if (!SetCursor(row, column))
		{
			return false;
		}

		if (!WriteData(string[i]))
		{
			return false;
		}

		column++;
		column %= 16;
	}

	return true;


}

extern "C" void lumex_lcd_timer_interrupt(TIM_HandleTypeDef* timer, osMessageQueueId_t timInterruptCallbackqHandle)
{
	HAL_StatusTypeDef status = HAL_TIM_Base_Stop_IT(timer);
	HAL_GPIO_WritePin(LUMEX_LCD_EN_GPIO_Port, LUMEX_LCD_EN_Pin, GPIO_PIN_RESET);
	osMessageQueuePut(timInterruptCallbackqHandle, &status, 0, 0);

}

extern "C" void lumex_lcd_main(TIM_HandleTypeDef* timer, osMessageQueueId_t lumexLcdToSessionControllerqHandle, osMessageQueueId_t timInterruptCallbackqHandle)
{
	LumexLCD lcd = LumexLCD(timer, lumexLcdToSessionControllerqHandle, timInterruptCallbackqHandle);

	if (!lcd.Init())
	{
		return;
	}


	lcd.Run();
}

//void LumexLCD::ToggleBlink(bool enable)
//{
//	if (enable)
//	{
//		if (!WriteCommand(0x0d)) // Display ON, Cursor OFF, Blink ON
//		{
//			return false;
//		}
//	}
//
//	else
//	{
//		if (!WriteCommand(0x0c))
//		{
//			return false;
//		}
//	}
//
//	return true;
//
//}




