#ifndef INC_MESSAGEPASSING_CIRCULAR_BUFFERS_H_
#define INC_MESSAGEPASSING_CIRCULAR_BUFFERS_H_

#include "messages.h"

#include "Config/config.h"
#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif

extern size_t optical_encoder_circular_buffer_index_writer;
extern size_t forcesensor_circular_buffer_index_writer;
extern size_t bpm_circular_buffer_index_writer;

extern optical_encoder_output_data optical_encoder_circular_buffer[OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE];
extern forcesensor_output_data forcesensor_circular_buffer[FORCESENSOR_CIRCULAR_BUFFER_SIZE];
extern bpm_output_data bpm_circular_buffer[BPM_CIRCULAR_BUFFER_SIZE];

extern size_t task_error_circular_buffer_index_writer;

extern task_errors task_error_circular_buffer[TASK_ERROR_CIRCULAR_BUFFER_SIZE];

#ifdef __cplusplus
}
#endif

#endif // INC_MESSAGEPASSING_CIRCULAR_BUFFERS_H_
