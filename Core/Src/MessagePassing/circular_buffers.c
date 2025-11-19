#include "MessagePassing/circular_buffers.h"

circular_buffer_config optical_encoder_circular_buffer_config = {
    .writerIndex = 0,
    .readerIndex = 0,
    .size = OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE
};

circular_buffer_config forcesensor_circular_buffer_config = {
    .writerIndex = 0,
    .readerIndex = 0,
    .size = FORCESENSOR_CIRCULAR_BUFFER_SIZE
};

circular_buffer_config bpm_circular_buffer_config = {
    .writerIndex = 0,
    .readerIndex = 0,
    .size = BPM_CIRCULAR_BUFFER_SIZE
};


optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];
forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];
bpm_output_data bpm_circular_buffer[BPM_CIRCULAR_BUFFER_SIZE];