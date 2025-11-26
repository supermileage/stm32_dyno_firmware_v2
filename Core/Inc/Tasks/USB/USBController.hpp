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
        void SafeIncrement(size_t);
    private:
        CircularBufferReader<optical_encoder_output_data> _buffer_reader_oe;
        CircularBufferReader<forcesensor_output_data> _buffer_reader_fs;
        CircularBufferReader<bpm_output_data> _buffer_reader_bpm;

        optical_encoder_output_data oe_output; // Pointers to each of these structs
        forcesensor_output_data fs_output;
        bpm_output_data bpm_output;

        uint8_t _txBuffer[USB_CDC_TX_BUFFER_SIZE];
        int _txBufferIndex = 0;
};
