#include "cmsis_os2.h"
#include "TimeKeeping/timestamps.h"
#include "MessagePassing/circular_buffers.h"
#include "CircularBufferReader.hpp"

class USBController
{
    public:
        USBController();
        ~USBController() = default; // Destructor

        bool Init();
        void Run();
    private:
        CircularBufferReader<optical_encoder_output_data> _buffer_reader_oe;
        CircularBufferReader<forcesensor_output_data> _buffer_reader_fs;
        CircularBufferReader<bpm_output_data> _buffer_reader_bpm;

        uint8_t _txBuffer[USB_CDC_TX_BUFFER_SIZE];
}