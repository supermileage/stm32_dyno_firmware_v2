#ifndef INC_TASKS_USB_USBCONTROLLER_HPP_
#define INC_TASKS_USB_USBCONTROLLER_HPP_

#include <array>
#include <algorithm>

#include "cmsis_os2.h"

#include "TimeKeeping/timestamps.h"

#include "MessagePassing/msgq_messages.h"
#include "MessagePassing/circular_buffers.h"
#include "MessagePassing/osqueue_helpers.h"
#include "MessagePassing/errors.h"

#include "Config/config.h"
#include "CircularBufferReader.hpp"



class USBController
{
    public:
        USBController(osMessageQueueId_t sessionControllerToUsbController, osMessageQueueId_t taskMonitorToUsbControllerHandle);
        ~USBController() = default; // Destructor

        bool Init();
        void Run();
    private:
        void AddToBuffer(void*, size_t);    
        bool BufferFull(std::size_t msgSize);
        void ProcessErrorsAndWarnings();

        template <typename T>
        void ProcessTaskData(CircularBufferReader<T>& bufferReader, task_ids_t messageId)
        {            
            T data; // Temporary variable to hold the data
            while (bufferReader.HasData()) { // Check if data is available
                // Ensure the buffer is not full before adding data
                if (BufferFull(sizeof(T))) {
                    while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
                        osDelay(1);
                    }
                    _txBufferIndex = 0;
                }

                if (bufferReader.GetElementAndIncrementIndex(data)) {
                    usb_msg_header_t header = 
                    {
                        .msg_type = USB_MSG_STREAM,
                        .module_id = messageId,
                        .payload_len = sizeof(T)
                    };
                    AddToBuffer(&header, sizeof(usb_msg_header_t));
                    AddToBuffer(&data, sizeof(T));
                }
            }
        }

        template <typename T>
        void ProcessTaskData(osMessageQueueId_t msgqHandle, task_ids_t messageId)
        {            
            T data; // Temporary variable to hold the data
            while (osMessageQueueGet(msgqHandle, &data, 0, 0) == osOK) { // Check if data is available
                // Ensure the buffer is not full before adding data
                if (BufferFull(sizeof(T))) {
                    while (CDC_Transmit_FS(_txBuffer, _txBufferIndex) == USBD_BUSY) {
                        osDelay(1);
                    }
                    _txBufferIndex = 0;
                }

                usb_msg_header_t header = 
                {
                    .msg_type = USB_MSG_STREAM,
                    .module_id = messageId,
                    .payload_len = sizeof(T)
                };
                AddToBuffer(&header, sizeof(usb_msg_header_t));
                AddToBuffer(&data, sizeof(T));
                
            }
        }

        CircularBufferReader<task_error_data> _task_errors_buffer_reader;
    
        CircularBufferReader<optical_encoder_output_data> _buffer_reader_optical_encoder;
        CircularBufferReader<forcesensor_output_data> _buffer_reader_forcesensor;
        CircularBufferReader<bpm_output_data> _buffer_reader_bpm;

        osMessageQueueId_t _taskMonitorToUsbControllerHandle;
        osMessageQueueId_t _sessionControllerToUsbController;

        uint8_t _txBuffer[USB_TX_BUFFER_SIZE];
        int _txBufferIndex = 0;
};

#endif // INC_TASKS_USB_USBCONTROLLER_HPP_
