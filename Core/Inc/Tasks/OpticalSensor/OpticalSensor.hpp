#ifndef INC_OPTICALSENSOR_HPP_
#define INC_OPTICALSENSOR_HPP_

#include <cstdint>

#include "cmsis_os2.h"

#include "Config/hal_instances.h"

#include "TimeKeeping/timestamps.h"

#include "MessagePassing/circular_buffers.h"

#include "CircularBufferWriter.hpp"

class OpticalSensor
{
public:
    OpticalSensor(osMessageQueueId_t sessionControllerToOpticalSensorHandle);

    bool Init();
    void Run();
    
private:
    float GetRPM(uint32_t timerCounterDifference);
    float GetAngularVelocity(uint32_t timerCounterDifference);
    float GetAngularAcceleration(uint32_t timerCounterDifference, uint32_t prevTimerCounterDifference);





	void ToggleOpticalEncoder(bool enable);

    CircularBufferWriter<optical_encoder_output_data> _buffer_writer;

    osMessageQueueId_t _sessionControllerToOpticalSensorHandle;

    bool _opticalEncoderEnabled;
};

#endif //
