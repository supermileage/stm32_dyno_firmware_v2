#ifndef INC_MESSAGEPASSING_CIRCULAR_BUFFERS_H_
#define INC_MESSAGEPASSING_CIRCULAR_BUFFERS_H_

#include "messages.h"

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  {
    uint32_t writerIndex;
    uint32_t readerIndex;
    const uint32_t size;
} circular_buffer_config;

extern circular_buffer_config optical_encoder_circular_buffer_config;
extern circular_buffer_config forcesensor_circular_buffer_config;
extern circular_buffer_config bpm_circular_buffer_config;


extern optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];
extern forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];
extern bpm_output_data bpm_circular_buffer[BPM_CIRCULAR_BUFFER_SIZE];

#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_CIRCULAR_BUFFERS_H_
