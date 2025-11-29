#ifndef INC_USB_USB_HPP_
#define INC_USB_USB_HPP_

#include "cmsis_os2.h"
#include "TimeKeeping/timestamps.h"
#include "MessagePassing/messages.h"
#include "config.h"
#include "CircularBufferReader.hpp"

class USBController
{
    public:
        USBController(osMessageQueueId_t);
        ~USBController() = default; // Destructor

        bool Init();
        void Run();
    private:
        void SendOutputToUSB(size_t);
        size_t EnoughSpace(size_t, size_t);
    
        CircularBufferReader<optical_encoder_output_data> _buffer_reader_oe;
        CircularBufferReader<forcesensor_output_data> _buffer_reader_fs;
        CircularBufferReader<bpm_output_data> _buffer_reader_bpm;

        optical_encoder_output_data _oe_output; // Structs containing each output type
        forcesensor_output_data _fs_output;
        bpm_output_data _bpm_output;

        osMessageQueueId_t _sessionControllerToUsbController;
        uint8_t _txBuffer[TX_BUFFER_SIZE];
        int _txBufferIndex = 0;
};

#endif // INC_USB_USB_HPP_
