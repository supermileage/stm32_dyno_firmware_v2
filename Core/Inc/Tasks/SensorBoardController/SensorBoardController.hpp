#ifndef INC_TASKS_SENSORBOARDCONTROLLER_SENSORBOARDCONTROLLER_HPP_
#define INC_TASKS_SENSORBOARDCONTROLLER_SENSORBOARDCONTROLLER_HPP_

#include "main.h"

#include "cmsis_os2.h"

#include "Config/config.h"
#include "Config/debug.h"

#include "../../../../stm32_dyno_firmware_sensor_board/Core/Inc/Messages/messages_public.h"
#include "../../../../stm32_dyno_firmware_sensor_board/Drivers/ADS1115/ADS1115_defines.h"

#include "MessagePassing/messages_public.h"
#include "MessagePassing/messages_private.h"

#include "CircularBufferWriter.hpp"

#include "TimeKeeping/timestamps.h"

#include "MessagePassing/osqueue_helpers.h"

class SensorBoardController 
{
    public:
        SensorBoardController(osMessageQueueId_t sessionControllerToSensorBoardControllerHandle, osMutexId_t usart1Mutex);

        bool Init();
        void Run();
    private:

        bool InitOpticalSensor();
        bool InitForceSensorADC();
        bool InitForceSensorADS1115();

        // Overloaded methods for UART transmission and reception with mutex protection
        HAL_StatusTypeDef HAL_UART_Transmission_WithMutex(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout, HAL_StatusTypeDef (*uartFunc)(UART_HandleTypeDef *, uint8_t *, uint16_t, uint32_t));
        HAL_StatusTypeDef HAL_UART_Transmission_WithMutex(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, HAL_StatusTypeDef (*uartFunc)(UART_HandleTypeDef *, uint8_t *, uint16_t));

        bool HasError(child_board_task_error_data& error_data);

        float GetForce(uint16_t raw_value);
        float GetAngularVelocity(uint32_t num_posedges, uint32_t timer_counter_value);
        
        CircularBufferWriter<task_error_data> _task_error_buffer_writer;
        CircularBufferWriter<optical_encoder_output_data> _buffer_writer_optical_encoder;
        CircularBufferWriter<forcesensor_output_data> _buffer_writer_forcesensor;
        osMessageQueueId_t _sessionControllerToSensorBoardControllerHandle;
        osMutexId_t _usart1Mutex;

        bool _enabled;

};

#endif /* INC_TASKS_SENSORBOARDCONTROLLER_SENSORBOARDCONTROLLER_HPP_ */