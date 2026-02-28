# BPM Task

## Overview
The BPM (Brushless Permanent Magnet) task is responsible for controlling the PWM (Pulse Width Modulation) signals to manage the operation of a motor. It interacts with other tasks via message queues and ensures proper duty cycle adjustments and error handling.

---

## Table of Contents
1. [BPM.hpp](#bpmhpp)
2. [BPM.cpp](#bpmcpp)
3. [bpm_main.h](#bpm_mainh)

---

## BPM.hpp

### Overview
This file defines the `BPM` class, which encapsulates the functionality of the BPM task. It includes methods for initialization, PWM control, and error handling.

### Class Members

#### Private Members
- `_data_buffer_writer`: Writes duty cycle data to a circular buffer.
- `_task_error_buffer_writer`: Writes error data to a circular buffer.
- `_fromSCHandle`: Message queue handle for the Session Controller.
- `_fromPIDHandle`: Message queue handle for the PID controller.
- `_prevBpmCtrlEnabled`: Tracks the previous state of the PWM signal.

#### Private Methods
- `bool TogglePWM(bool enable)`: Starts or stops the PWM signal based on the `enable` parameter.
- `bool SetDutyCycle(uint8_t dutyCyclePercent)`: Adjusts the duty cycle of the PWM signal.
- `bool LogDutyCycle(uint8_t dutyCyclePercent, uint32_t timestamp)`: Logs the duty cycle and timestamp to a circular buffer.
- `bool StallIfIsBufferFull(bool bufferFull)`: Logs errors encountered during PWM operations to a circular buffer.

#### Public Methods
- `BPM(osMessageQueueId_t sessionControllerToBpmHandle, osMessageQueueId_t pidToBpmHandle)`: Constructor that initializes the BPM task with message queue handles.
- `~BPM()`: Default destructor.
- `bool Init()`: Initializes the BPM task.
- `void Run()`: The main loop of the BPM task.

---

## BPM.cpp

### Overview
This file implements the `BPM` class methods. It handles initialization, PWM control, and error handling.

### Initialization

#### `Init()`
The `Init` method performs any necessary setup. Currently, it always returns `true`.

### PWM Control

#### `TogglePWM(bool enable)`
Starts or stops the PWM signal based on the `enable` parameter. Ensures that the PWM signal is only started or stopped when necessary and logs errors to a circular buffer if the operation fails.

#### `SetDutyCycle(uint8_t dutyCyclePercent)`
Adjusts the duty cycle of the PWM signal. Ensures the duty cycle remains within predefined limits (`MIN_DUTY_CYCLE_PERCENT` and `MAX_DUTY_CYCLE_PERCENT`) and updates the timer's compare register accordingly.

#### `LogDutyCycle(uint8_t dutyCyclePercent, uint32_t timestamp)`
Logs the duty cycle and timestamp to a circular buffer.

### Main Loop

#### `Run()`
The main loop of the BPM task. It performs the following steps:
1. Waits for messages from the Session Controller queue.
2. Processes commands such as:
   - `READ_FROM_PID`: Enables reading duty cycle updates from the PID controller.
   - `START_PWM`: Starts the PWM signal with a specified duty cycle.
   - `STOP_PWM`: Stops the PWM signal.
3. If reading from the PID controller is enabled, retrieves the latest duty cycle value and updates the PWM signal accordingly.
4. Logs the duty cycle and timestamp to a circular buffer.
5. Delays the task for a predefined period (`BPM_TASK_OSDELAY`).

---

## bpm_main.h

### Overview
This file declares the `bpm_main()` function, which is the entry point for the BPM task.

### `bpm_main()`
The entry point for the BPM task. It:
1. Constructs a `BPM` instance.
2. Calls `Init()` to initialize the BPM task.
3. Calls `Run()` to start the main loop.

---

## Summary of Inter-Task Communication

```
BPM
    ├── ← Session Controller queue   (commands to start/stop PWM or read from PID)
    ├── ← PID Controller queue       (duty cycle updates)
    └── → Task Error Buffer          (errors encountered during PWM operations)
```

---

## Conclusion
The BPM task is a critical component for motor control, providing robust PWM management and error handling. Its modular design and use of message queues make it easy to integrate with other tasks in the system.