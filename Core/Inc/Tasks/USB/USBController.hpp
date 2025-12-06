#ifndef INC_USB_USB_HPP_
#define INC_USB_USB_HPP_

#include "cmsis_os2.h"
#include "TimeKeeping/timestamps.h"
#include "MessagePassing/messages.h"
#include "config.h"
#include "CircularBufferReader.hpp"

enum class USBOpcode : uint8_t {
    OPTICAL_ENCODER = 0,
    FORCESENSOR     = 1,
    BPM             = 2
};

class USBController
{
    public:
        USBController(osMessageQueueId_t);
        ~USBController() = default; // Destructor

        bool Init();
        void Run();
    private:
        void AddToBuffer(void*, size_t, size_t);
        bool SendOutputIfBufferFull(size_t, size_t);
    
        CircularBufferReader<optical_encoder_output_data> _buffer_reader_oe;
        CircularBufferReader<forcesensor_output_data> _buffer_reader_fs;
        CircularBufferReader<bpm_output_data> _buffer_reader_bpm;

        optical_encoder_output_data _opticalEncoderOutput; // Structs containing each output type
        forcesensor_output_data _forceSensorOutput;
        bpm_output_data _bpmOutput;
        size_t _standardSize;

        osMessageQueueId_t _sessionControllerToUsbController;
        uint8_t _txBuffer[USB_TX_BUFFER_SIZE];
        int _txBufferIndex = 0;
};

#endif // INC_USB_USB_HPP_
