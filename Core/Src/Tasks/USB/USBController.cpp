#include <Tasks/USB/usbcontroller_main.h>
#include <Tasks/USB/USBController.hpp>

extern size_t optical_encoder_circular_buffer_index_writer;
extern size_t forcesensor_circular_buffer_index_writer;
extern size_t bpm_circular_buffer_index_writer;

extern optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];
extern forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];
extern bpm_output_data bpm_circular_buffer[BPM_CIRCULAR_BUFFER_SIZE];

extern size_t task_error_circular_buffer_index_writer;
extern task_error_data task_error_circular_buffer[TASK_ERROR_CIRCULAR_BUFFER_SIZE];

extern uint8_t usb_controller_rx_buffer[USB_CONTROLLER_RX_BUFFER_SIZE];

USBController::USBController(osMessageQueueId_t sessionControllerToUsbController, osMessageQueueId_t taskMonitorToUsbControllerHandle)
    : _task_errors_buffer_reader(task_error_circular_buffer, &task_error_circular_buffer_index_writer, TASK_ERROR_CIRCULAR_BUFFER_SIZE),
        _buffer_reader_optical_encoder(optical_encoder_circular_buffer, &optical_encoder_circular_buffer_index_writer, BPM_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_forcesensor(forcesensor_circular_buffer, &forcesensor_circular_buffer_index_writer, FORCESENSOR_CIRCULAR_BUFFER_SIZE),
      _buffer_reader_bpm(bpm_circular_buffer, &bpm_circular_buffer_index_writer, OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE),
      _taskMonitorToUsbControllerHandle(taskMonitorToUsbControllerHandle),
      _sessionControllerToUsbController(sessionControllerToUsbController),
      _txBuffer{},
      _txBufferIndex(0)
{}

bool USBController::Init()
{
	return true;
}

void USBController::ReceiveAppAck()
{
    while (true)
    {
        // Attempt to receive data over USB
        if (usb_controller_rx_buffer[0] == 'O' && usb_controller_rx_buffer[1] == 'K' && usb_controller_rx_buffer[2] == '\0')
        {
            break;
        }

        osDelay(10); // Small delay to prevent busy-waiting
    }
}

void USBController::Run()
{
    bool enableUSB = false;

    // Wait for "OK" before starting data transmission
    ReceiveAppAck();

    while (1)
    {
        GetLatestFromQueue(
            _sessionControllerToUsbController,
            &enableUSB,
            sizeof(enableUSB),
            enableUSB ? 0 : osWaitForever
        );

        if (!enableUSB)
        {
            continue;
        }



        #if !defined(OPTICAL_ENCODER_TASK_ENABLE)
        #error "OPTICAL_ENCODER_TASK_ENABLE must be defined"
        #elif (OPTICAL_ENCODER_TASK_ENABLE == 1)
        // Process optical encoder data
        ProcessTaskData(_buffer_reader_optical_encoder, TASK_ID_OPTICAL_ENCODER);
        #endif 

        #if !defined(FORCE_SENSOR_ADC_TASK_ENABLE) || !defined(FORCE_SENSOR_ADS1115_TASK_ENABLE)
        #error "FORCE_SENSOR_TASK_ENABLE must be defined"
        #elif (FORCE_SENSOR_TASK_ENABLE == 1)
        // Process force sensor data
        ProcessTaskData(_buffer_reader_forcesensor, TASK_ID_FORCE_SENSOR);
        #endif 

        #if !defined(BPM_CONTROLLER_TASK_ENABLE)
        #error "BPM_CONTROLLER_TASK_ENABLE must be defined"
        #elif (BPM_CONTROLLER_TASK_ENABLE == 1)
        // Process BPM data
        ProcessTaskData(_buffer_reader_bpm, TASK_ID_BPM_CONTROLLER);
        #endif

        #if !defined(TASK_MONITOR_TASK_ENABLE)
        #error "TASK_MONITOR_TASK_ENABLE must be defined"
        #elif (TASK_MONITOR_TASK_ENABLE == 1)
        // Process Task Monitor data
        ProcessTaskData<task_monitor_output_data>(_taskMonitorToUsbControllerHandle, TASK_ID_TASK_MONITOR);
        #endif

        ProcessErrorsAndWarnings();

        

        if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
            continue;
        }
        _txBufferIndex = 0;

        osDelay(USB_TASK_OSDELAY);
    }
}



// void USBController::Run()
// {
// 	uint8_t msg[6];

// 	for (;;)
// 	{
// 	    msg[0] = 'B';
// 	    msg[1] = HAL_GPIO_ReadPin(BTN_BRAKE_GPIO_Port,  BTN_BRAKE_Pin)  ? '1' : '0';
// 	    msg[2] = 'S';
// 	    msg[3] = HAL_GPIO_ReadPin(BTN_SELECT_GPIO_Port, BTN_SELECT_Pin) ? '1' : '0';
// 	    msg[4] = 'K';
// 	    msg[5] = '\n';

// 	    while (CDC_Transmit_FS(msg, sizeof(msg)) == USBD_BUSY)
// 	    {
// 	        osDelay(1);
// 	    }

// 	    osDelay(100);
// 	}

// }

void USBController::MockMessages(const bool forever)
{

    uint32_t timestamp = 0;
    usb_msg_header_t usb_header{};

    // Wait for "OK" before starting data transmission
    ReceiveAppAck();
    
    while(forever)
    {
        #if !defined(OPTICAL_ENCODER_TASK_ENABLE)
        #error "OPTICAL_ENCODER_TASK_ENABLE must be defined"
        #elif (OPTICAL_ENCODER_TASK_ENABLE == 1)
        static float angular_velocity = 0.0f;
        static uint32_t optical_raw_value = 0;
        static float angular_acceleration = 0.0f;
        optical_encoder_output_data mock_data = {
            .timestamp = timestamp++,
            .angular_velocity = angular_velocity++,
            .raw_value = optical_raw_value++,
            .angular_acceleration = angular_acceleration++
        };
        // Process optical encoder data
        usb_header.msg_type = USB_MSG_STREAM;
        usb_header.module_id = TASK_ID_OPTICAL_ENCODER;
        usb_header.payload_len = sizeof(optical_encoder_output_data);

        AddToBuffer<usb_msg_header_t>(&usb_header, sizeof(usb_msg_header_t));
        AddToBuffer<optical_encoder_output_data>(&mock_data, sizeof(optical_encoder_output_data));
        #endif

        #if !defined(FORCE_SENSOR_ADS1115_TASK_ENABLE) || !defined(FORCE_SENSOR_ADC_TASK_ENABLE)
        #error "FORCE_SENSOR_TASK_ENABLE must be defined"
        #elif (FORCE_SENSOR_ADS1115_TASK_ENABLE || FORCE_SENSOR_ADC_TASK_ENABLE)
        static float force = 0.0f;
        static uint32_t fs_raw_value = 0;
        forcesensor_output_data mock_fs_data = {
            .timestamp = timestamp++,
            .force = force++,
            .raw_value = fs_raw_value++
        };
        usb_header.msg_type = USB_MSG_STREAM;
        usb_header.module_id = TASK_ID_FORCE_SENSOR;
        usb_header.payload_len = sizeof(forcesensor_output_data);

        AddToBuffer<usb_msg_header_t>(&usb_header, sizeof(usb_msg_header_t));
        AddToBuffer<forcesensor_output_data>(&mock_fs_data, sizeof(forcesensor_output_data));
        #endif

        #if !defined(BPM_CONTROLLER_TASK_ENABLE)
        #error "BPM_CONTROLLER_TASK_ENABLE must be defined"
        #elif (BPM_CONTROLLER_TASK_ENABLE == 1)
        static float duty_cycle = 0.0f;
        static uint32_t bpm_raw_value = 0;
        bpm_output_data mock_bpm_data = {
            .timestamp = timestamp++,
            .duty_cycle = duty_cycle++,
            .raw_value = bpm_raw_value++
        };
        usb_header.msg_type = USB_MSG_STREAM;
        usb_header.module_id = TASK_ID_BPM_CONTROLLER;
        usb_header.payload_len = sizeof(bpm_output_data);
        AddToBuffer<usb_msg_header_t>(&usb_header, sizeof(usb_msg_header_t));
        AddToBuffer<bpm_output_data>(&mock_bpm_data, sizeof(bpm_output_data));
        #endif

        #if !defined(TASK_MONITOR_TASK_ENABLE)
        #error "TASK_MONITOR_TASK_ENABLE must be defined"
        #elif (TASK_MONITOR_TASK_ENABLE == 1)
        static uint32_t task_monitor_raw_value = 0;

        task_monitor_output_data mock_tm_data = {
            .timestamp = timestamp++,
            .task_id = TASK_ID_NO_TASK,
            .task_state = 0,
            .free_bytes = task_monitor_raw_value++
        };
        usb_header.msg_type = USB_MSG_STREAM;
        usb_header.module_id = TASK_ID_TASK_MONITOR;
        usb_header.payload_len = sizeof(task_monitor_output_data);
        AddToBuffer<usb_msg_header_t>(&usb_header, sizeof(usb_msg_header_t));
        AddToBuffer<task_monitor_output_data>(&mock_tm_data, sizeof(task_monitor_output_data));
        #endif

        if (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
            continue;
        }
        _txBufferIndex = 0;

        osDelay(USB_TASK_OSDELAY);
    }
}

void USBController::ProcessErrorsAndWarnings()
{
    while(_task_errors_buffer_reader.HasData()) 
    {

        StallIfIsBufferFull(IsBufferFull(sizeof(task_error_data)));

        task_error_data error_data;
        if (_task_errors_buffer_reader.GetElementAndIncrementIndex(error_data)) {
            usb_msg_header_t header = 
            {
                .msg_type = (error_data.error_id >= WARNING_ENUM_OFFSET) ? USB_MSG_WARNING : USB_MSG_ERROR,
                .module_id = TASK_ID_NO_TASK,
                .payload_len = sizeof(task_error_data)
            };

            AddToBuffer<usb_msg_header_t>(&header, sizeof(header));
            AddToBuffer<task_error_data>(&error_data, sizeof(task_error_data));
        }
    }
}

void USBController::StallIfIsBufferFull(bool bufferFull)
{
    // Ensure the buffer is not full before adding data
    if (bufferFull) {
        while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
            osDelay(1);
        }
        _txBufferIndex = 0;
    }
}

bool USBController::IsBufferFull(std::size_t msgSize)
{   
    if (_txBufferIndex + sizeof(usb_msg_header_t) + msgSize >= USB_TX_BUFFER_SIZE) {
        return true;   
    }
	return false;

}

extern "C" void usbcontroller_main(osMessageQueueId_t sessionControllerToUsbController, osMessageQueueId_t taskMonitorToUsbControllerHandle)
{
	USBController usb = USBController(sessionControllerToUsbController, taskMonitorToUsbControllerHandle);

	if (!usb.Init())
	{
		 osThreadSuspend(osThreadGetId());;
	}

    #if !defined(DEBUG_USB_CONTROLLER_MOCK_MESSAGES)
    #error "DEBUG_USB_CONTROLLER_MOCK_MESSAGES must be defined"
    #elif (DEBUG_USB_CONTROLLER_MOCK_MESSAGES == 1)
    usb.MockMessages();
    #else
	usb.Run();
    #endif
}
