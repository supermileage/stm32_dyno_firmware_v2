#include "Tasks/SensorBoardController/sensorboardcontroller_main.h"
#include "Tasks/SensorBoardController/SensorBoardController.hpp"

#include <cstring>
#include "semphr.h"

// Wrapper to match uint8_t* function pointer signature (HAL_UART_Transmit uses const uint8_t*)
static HAL_StatusTypeDef HAL_UART_Transmit_Wrapper(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    return HAL_UART_Transmit(huart, pData, Size, Timeout);
}

#define LBF_TO_NEWTONS 4.44822f

extern UART_HandleTypeDef huart1;

extern size_t task_error_circular_buffer_index_writer;
extern task_error_data task_error_circular_buffer[TASK_ERROR_CIRCULAR_BUFFER_SIZE];
extern size_t optical_encoder_circular_buffer_index_writer;
extern optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];
extern size_t forcesensor_circular_buffer_index_writer;
extern forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];

extern osSemaphoreId_t sensorBoardUsartSemaphoreHandle;

child_board_usart_combined_data_t usart_input_data{};

SensorBoardController::SensorBoardController(osMessageQueueId_t sessionControllerToSensorBoardControllerHandle, osMutexId_t usart1Mutex) : 
                                            _task_error_buffer_writer(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
                                            _buffer_writer_optical_encoder(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
                                            _buffer_writer_forcesensor(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
                                            _sessionControllerToSensorBoardControllerHandle(sessionControllerToSensorBoardControllerHandle), 
                                            _usart1Mutex(usart1Mutex),
                                            _enabled(false)
{
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Wake task waiting for UART data
        xSemaphoreGiveFromISR(
            (SemaphoreHandle_t)sensorBoardUsartSemaphoreHandle,
            &xHigherPriorityTaskWoken
        );

        HAL_UART_Receive_IT(&huart1, (uint8_t*) &usart_input_data, sizeof(usart_input_data));

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

bool SensorBoardController::Init()
{
    #if (!defined(OPTICAL_ENCODER_TASK_ENABLE)) 
        #error "OPTICAL_ENCODER_TASK_ENABLE must be defined in debug.h"
    #elif (OPTICAL_ENCODER_TASK_ENABLE)
        while (!InitOpticalSensor())
        {
            osDelay(TASK_WARNING_RETRY_OSDELAY);
        }
    #endif 

    #if (!defined(FORCE_SENSOR_ADC_TASK_ENABLE)) 
        #error "FORCE_SENSOR_ADC_TASK_ENABLE must be defined in debug.h"
    #elif (FORCE_SENSOR_ADC_TASK_ENABLE)
        while (!InitForceSensorADC())
        {
            osDelay(TASK_WARNING_RETRY_OSDELAY);
        }
    #endif

    #if (!defined(FORCE_SENSOR_ADS1115_TASK_ENABLE)) 
        #error "FORCE_SENSOR_ADS1115_TASK_ENABLE must be defined in debug.h"
    #elif (FORCE_SENSOR_ADS1115_TASK_ENABLE)
        while (!InitForceSensorADS1115())
        {
            osDelay(TASK_WARNING_RETRY_OSDELAY);
        }
    #endif 

    if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&usart_input_data, sizeof(usart_input_data), HAL_UART_Receive_IT) != HAL_OK)
    {
        task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(ERROR_SENSOR_BOARD_CONTROLLER_UART_INTERRUPT_START_FAILURE));

        _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);

        return false;
    }

    return true;
}

void SensorBoardController::Run()
{
    while(1)
    {
        osDelay(SENSOR_BOARD_CONTROLLER_TASK_OSDELAY);
        
        GetLatestFromQueue(
            _sessionControllerToSensorBoardControllerHandle,
            &_enabled,
            sizeof(_enabled),
            _enabled ? 0 : osWaitForever
        );

        if (!_enabled)
        {
            continue;
        }

        if (HasError(usart_input_data.error_data))
        {
            osDelay(TASK_WARNING_RETRY_OSDELAY);
        }

        // Only process data if there is new data from the child board
        if (osSemaphoreAcquire(sensorBoardUsartSemaphoreHandle, osWaitForever) != osOK)
        {
            continue;
        }

        optical_encoder_output_data optical_data = 
        {
            .timestamp = get_timestamp(),
	        .angular_velocity = GetAngularVelocity(usart_input_data.sensor_data.optical_count_posedges, usart_input_data.sensor_data.optical_timer_counter_value),
	        .raw_value = 0U, // not currently used, needs num posedges and timer counter value to be useful so just set to 0 for now
	        .angular_acceleration = 0.0f // not currently used, would need to store previous angular velocity and timestamp to be useful so just set to 0 for now
        };

        _buffer_writer_optical_encoder.WriteElementAndIncrementIndex(optical_data);

        forcesensor_output_data force_data = 
        {
            .timestamp = get_timestamp(),
            .force = GetForce(static_cast<uint16_t>(usart_input_data.sensor_data.force_sensor_raw_value)),
            .raw_value = usart_input_data.sensor_data.force_sensor_raw_value
        };

        _buffer_writer_forcesensor.WriteElementAndIncrementIndex(force_data);


    }
}

bool SensorBoardController::InitOpticalSensor()
{
    uint32_t num_ack_retries = 0;
    bool ack_received = false;
    
    mother_board_to_child_board_usart_command_header_t command_header = 
    {
        .task_id = CHILD_BOARD_TASK_ID_OPTICAL_SENSOR,
        .enable = true,
        .command_payload_length = 0
    };

    while (ack_received == false)
    {
        
        if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&command_header, sizeof(command_header), HAL_MAX_DELAY, HAL_UART_Transmit_Wrapper) != HAL_OK)
        {
            task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_OPTICAL_SENSOR_CONFIG_WRITE_FAILURE));

            _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
            
            return false;
        }

        mother_board_to_child_board_usart_command_header_t received_command_header = 
        {
            .task_id = CHILD_BOARD_TASK_INVALID_TASK_ID,
            .enable = false,
            .command_payload_length = 0
        };

        num_ack_retries = 0;

        while (num_ack_retries < OPTICAL_SENSOR_ACK_MAX_RETRIES)
        {
            if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&received_command_header, sizeof(received_command_header), TASK_WARNING_RETRY_OSDELAY, HAL_UART_Receive) != HAL_OK)
            {
                task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_OPTICAL_SENSOR_CONFIG_ACK_FAILURE));

                _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
                num_ack_retries++;
            }
            else if (std::memcmp(&command_header, &received_command_header, sizeof(command_header)) == 0)
            {
                ack_received = true;
                break;
            }
            
        }

    }

    return true;

}

bool SensorBoardController::InitForceSensorADC()
{
    uint32_t num_ack_retries = 0;
    bool ack_received = false;
    
    mother_board_to_child_board_usart_command_header_t command_header = 
    {
        .task_id = CHILD_BOARD_TASK_ID_FORCE_SENSOR_ADC,
        .enable = true,
        .command_payload_length = 0
    };

    while(ack_received == false)
    {

    if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&command_header, sizeof(command_header), HAL_MAX_DELAY, HAL_UART_Transmit_Wrapper) != HAL_OK)
    {
        task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_FORCE_SENSOR_ADC_CONFIG_WRITE_FAILURE));

        _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
        
        return false;
    }

    mother_board_to_child_board_usart_command_header_t received_command_header = 
    {
        .task_id = CHILD_BOARD_TASK_INVALID_TASK_ID,
        .enable = false,
        .command_payload_length = 0

    };

    num_ack_retries = 0;

    while(num_ack_retries < FORCE_SENSOR_ADC_ACK_MAX_RETRIES)
    {
        if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&received_command_header, sizeof(received_command_header), TASK_WARNING_RETRY_OSDELAY, HAL_UART_Receive) != HAL_OK)
        {
            task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_FORCE_SENSOR_ADC_CONFIG_ACK_FAILURE));

            _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);
            num_ack_retries++;
        }
        else if (std::memcmp(&command_header, &received_command_header, sizeof(command_header)) == 0)
        {
            ack_received = true;
            break;
        }
    }

    }

    return true;
}

bool SensorBoardController::InitForceSensorADS1115()
{
    
    uint32_t num_ack_retries = 0;
    bool ack_received = false;
    
    child_board_force_sensor_ads1115_settings settings = 
    {
        .address = ADS1115_DEFAULT_ADDRESS, 
        .multiplexer = ADS1115_DEFAULT_MULTIPLEXER_SETTING,
        .comparator_mode = ADS1115_DEFAULT_COMPARATOR_MODE,
        .comparator_polarity = ADS1115_DEFAULT_COMPARATOR_POLARITY,
        .comparator_latch_enabled = ADS1115_DEFAULT_COMPARATOR_LATCH_ENABLED, 
        .comparator_queue_mode = ADS1115_DEFAULT_COMPARATOR_QUEUE_MODE, 
        .mode = ADS1115_DEFAULT_MODE, 
        .data_rate = ADS1115_SAMPLE_SPEED, 
        .gain = ADS1115_DEFAULT_GAIN 
    };
    
    mother_board_to_child_board_usart_command_header_t command_header = 
    {
        .task_id = CHILD_BOARD_TASK_ID_FORCE_SENSOR_ADS1115,
        .enable = true,
        .command_payload_length = sizeof(settings)
    };

    while(ack_received == false)
    {

        if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&command_header, sizeof(command_header), HAL_MAX_DELAY, HAL_UART_Transmit_Wrapper) != HAL_OK)
        {
            task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_FORCE_SENSOR_ADS1115_CONFIG_WRITE_FAILURE));

            _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);

            return false;
        }

        if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&settings, sizeof(settings), HAL_MAX_DELAY, HAL_UART_Transmit_Wrapper) != HAL_OK)
        {
            task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_FORCE_SENSOR_ADS1115_CONFIG_WRITE_FAILURE));

            _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);

            return false;
        }

        mother_board_to_child_board_usart_command_header_t received_command_header = 
        {
            .task_id = CHILD_BOARD_TASK_INVALID_TASK_ID,
            .enable = false,
            .command_payload_length = 0
        };

        num_ack_retries = 0;

        while(num_ack_retries < ADS1115_ACK_MAX_RETRIES)
        {

            if (HAL_UART_Transmission_WithMutex(&huart1, (uint8_t*)&received_command_header, sizeof(received_command_header), TASK_WARNING_RETRY_OSDELAY, HAL_UART_Receive) != HAL_OK)
            {
                task_error_data error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(WARNING_SENSOR_BOARD_CONTROLLER_FORCE_SENSOR_ADS1115_CONFIG_ACK_FAILURE));

                _task_error_buffer_writer.WriteElementAndIncrementIndex(error_data);

                num_ack_retries++;
            }
            else if (std::memcmp(&command_header, &received_command_header, sizeof(command_header)) == 0)
            {
                ack_received = true;
                break;
            }

        }

    }

    return true;
}

float SensorBoardController::GetAngularVelocity(uint32_t num_posedges, uint32_t timer_counter_value)
{
    // number of posedges / num apertures to get number of revolutions divided by microseconds * 10^6 to get seconds = revolutions per second. Multiply by 2 * pi to get radians per second.
    _Static_assert(NUM_APERTURES > 0, "NUM_APERTURES must be greater than 0 to avoid division by zero in GetAngularVelocity");
    if (timer_counter_value == 0)
    {
        return 0.0f; // to avoid division by zero, if timer counter value is zero then angular velocity is effectively zero
    }
    return (static_cast<float>(num_posedges) / NUM_APERTURES) / (static_cast<float>(timer_counter_value) / 1000000.0f) * 2 * M_PI;
}

static float ADS1115_GetMvPerCount(uint8_t pgaMode) {
  switch (pgaMode) {
    case ADS1115_PGA_6P144:
      return ADS1115_MV_6P144;
      break;
    case ADS1115_PGA_4P096:
      return  ADS1115_MV_4P096;
      break;
    case ADS1115_PGA_2P048:
      return ADS1115_MV_2P048;
      break;
    case ADS1115_PGA_1P024:
      return ADS1115_MV_1P024;
      break;
    case ADS1115_PGA_0P512:
      return ADS1115_MV_0P512;
      break;
    case ADS1115_PGA_0P256:
    case ADS1115_PGA_0P256B:
    case ADS1115_PGA_0P256C:
      return ADS1115_MV_0P256;
      break;
    default:
    	return 0.0;
  }
}

float SensorBoardController::GetForce(uint16_t raw_value)
{
    // mv per count * count / 1000 to get volts / supply voltage to get ratio * max force in lbf * lbf to newton
    return ADS1115_GetMvPerCount(ADS1115_DEFAULT_GAIN) * raw_value  / 1000 / ADS1115_FORCESENSOR_VOLTAGE * MAX_FORCE_LBF * LBF_TO_NEWTONS;
}



HAL_StatusTypeDef SensorBoardController::HAL_UART_Transmission_WithMutex(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout, HAL_StatusTypeDef (*uartFunc)(UART_HandleTypeDef *, uint8_t *, uint16_t, uint32_t))
{
    HAL_StatusTypeDef status;

    osMutexAcquire(_usart1Mutex, osWaitForever);
    status = uartFunc(huart, pData, Size, Timeout);
    osMutexRelease(_usart1Mutex);

    return status;
}

HAL_StatusTypeDef SensorBoardController::HAL_UART_Transmission_WithMutex(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, HAL_StatusTypeDef (*uartFunc)(UART_HandleTypeDef *, uint8_t *, uint16_t))
{
    HAL_StatusTypeDef status;

    osMutexAcquire(_usart1Mutex, osWaitForever);
    status = uartFunc(huart, pData, Size);
    osMutexRelease(_usart1Mutex);

    return status;
}

bool SensorBoardController::HasError(child_board_task_error_data& error_data)
{
    if (error_data.task_id == CHILD_BOARD_TASK_INVALID_TASK_ID)
    {
        return false;
    }

    task_error_data motherboard_error_data{};

    switch (error_data.task_id)
    {
        case CHILD_BOARD_TASK_ID_OPTICAL_SENSOR:
            motherboard_error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_OPTICAL_ENCODER, error_data.error_id);
            break;
        case CHILD_BOARD_TASK_ID_FORCE_SENSOR_ADC:
            motherboard_error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_FORCE_SENSOR_ADC, error_data.error_id);
            break;
        case CHILD_BOARD_TASK_ID_FORCE_SENSOR_ADS1115:
            motherboard_error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_FORCE_SENSOR_ADS1115, error_data.error_id);
            break;
        default:
            motherboard_error_data = PopulateTaskErrorDataStruct(get_timestamp(), TASK_ID_SENSOR_BOARD_CONTROLLER, static_cast<uint32_t>(ERROR_SENSOR_BOARD_CONTROLLER_INVALID_TASK_ID_RECEIVED));
                   
    }

    _task_error_buffer_writer.WriteElementAndIncrementIndex(motherboard_error_data);



    return true;
}



extern "C" void sensorboardcontroller_main(osMessageQueueId_t sessionControllerToSensorBoardControllerHandle, osMutexId_t usart1Mutex)
{
    SensorBoardController sensorBoardController = SensorBoardController(sessionControllerToSensorBoardControllerHandle, usart1Mutex);

    if (!sensorBoardController.Init())
    {
        osThreadSuspend(osThreadGetId());
    }

    sensorBoardController.Run();

    osThreadSuspend(osThreadGetId());
}