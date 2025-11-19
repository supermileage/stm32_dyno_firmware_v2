#ifndef INC_OPTICALSENSOR_HPP_
#define INC_OPTICALSENSOR_HPP_

#include "cmsis_os2.h"

#include "TimeKeeping/timestamps.h"

#include "MessagePassing/circular_buffers.h"

#include "CircularBufferWriter.hpp"

class OpticalSensor
{
public:
    OpticalSensor(TIM_HandleTypeDef* opticalTimer,
                  osMessageQueueId_t sessionControllerToOpticalSensorHandle);

    bool Init();
    void Run();
    float GetRPM(uint16_t timeDifference);

    void HandleCaptureInterrupt();
    void HandleOverflowInterrupt();

    static OpticalSensor* _instance;  // <-- public for ISR access

private:
	void ToggleOPS(bool enable);

    typedef struct {
        volatile uint32_t numOverflows;
        volatile uint32_t timestamp;
        volatile uint16_t IC_Value1;
        volatile uint16_t IC_Value2;
        volatile uint16_t timeDifference;
    } optical_encoder_input_data;

    CircularBufferWriter<optical_encoder_output_data> _buffer_writer;
    TIM_HandleTypeDef* _opticalTimer;
    osMessageQueueId_t _sessionControllerToOpticalSensorHandle;

    optical_encoder_input_data _optical;
    bool _opEcdrEnabled;
};

#endif //
