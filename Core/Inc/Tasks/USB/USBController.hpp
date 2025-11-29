#include "cmsis_os2.h"
#include "TimeKeeping/timestamps.h"
#include "MessagePassing/messages.h"
#include "config.h"
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

        optical_encoder_output_data _oe_output; // Structs containing each output type
        forcesensor_output_data _fs_output;
        bpm_output_data _bpm_output;

        uint8_t _txBuffer[APP_RX_DATA_SIZE];
        int _txBufferIndex = 0;
};
