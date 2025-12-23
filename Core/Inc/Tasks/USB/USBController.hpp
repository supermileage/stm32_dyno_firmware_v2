#ifndef INC_USB_USB_HPP_
#define INC_USB_USB_HPP_

#include <array>
#include <algorithm>

#include "cmsis_os2.h"

#include "TimeKeeping/timestamps.h"

#include "MessagePassing/messages.h"
#include "MessagePassing/circular_buffers.h"
#include "MessagePassing/osqueue_helpers.h"
#include "MessagePassing/errors.h"

#include "Config/config.h"
#include "CircularBufferReader.hpp"



class USBController
{
    public:
        enum class USBMessageIdentifier : uint8_t {
            TASK_ERROR = 0,
            TASK_WARNING = 1,
            TASK_OPTICAL_ENCODER = 10,
            TASK_FORCESENSOR     = 11,
            TASK_BPM             = 12
        };
        USBController(osMessageQueueId_t);
        ~USBController() = default; // Destructor

        bool Init();
        void Run();
    private:
        void AddToBuffer(void*, size_t);    
        void PadBuffer(size_t);
        bool BlockAndSendIfBufferFull();
        void ProcessErrorsAndWarnings();

        template <typename T>
        void ProcessTaskData(CircularBufferReader<T>& bufferReader, USBController::USBMessageIdentifier messageId)
        {
            T data; // Temporary variable to hold the data

            while (bufferReader.HasData()) { // Check if data is available
                // Ensure the buffer is not full before adding data
                if (!BlockAndSendIfBufferFull()) {
                    break;
                }

                if (bufferReader.GetElementAndIncrementIndex(data)) {
                    uint8_t op = static_cast<uint8_t>(messageId);
                    AddToBuffer(&op, sizeof(USBController::USBMessageIdentifier));
                    AddToBuffer(&data, sizeof(T));

                    size_t messageIndex = sizeof(USBController::USBMessageIdentifier) + sizeof(T);
                    PadBuffer(messageIndex);
                }
            }
        }

        CircularBufferReader<task_errors> _task_errors_buffer_reader;
    
        CircularBufferReader<optical_encoder_output_data> _buffer_reader_optical_encoder;
        CircularBufferReader<forcesensor_output_data> _buffer_reader_forcesensor;
        CircularBufferReader<bpm_output_data> _buffer_reader_bpm;

        const size_t _maxMsgSize;

        osMessageQueueId_t _sessionControllerToUsbController;
        uint8_t _txBuffer[USB_TX_BUFFER_SIZE];
        int _txBufferIndex = 0;
};

#endif // INC_USB_USB_HPP_
