# Sensor Board Controller Task

## Overview
The Sensor Board Controller task is responsible for managing the communication and data processing for the sensor board. It initializes and controls various sensors, including optical encoders and force sensors, and communicates with the main board via UART. The task ensures proper synchronization and error handling during sensor operations.

---

## Table of Contents
1. [SensorBoardController.hpp](#sensorboardcontrollerhpp)
2. [SensorBoardController.cpp](#sensorboardcontrollercpp)
3. [sensorboardcontroller_main.h](#sensorboardcontroller_mainh)

---

## SensorBoardController.hpp

### Overview
This file defines the `SensorBoardController` class, which encapsulates the functionality of the Sensor Board Controller task. It includes methods for initialization, sensor management, and UART communication.

### Class Members

#### Private Members
- `_task_error_buffer_writer`: Writes error data to a circular buffer.
- `_buffer_writer_optical_encoder`: Writes optical encoder data to a circular buffer.
- `_buffer_writer_forcesensor`: Writes force sensor data to a circular buffer.
- `_sessionControllerToSensorBoardControllerHandle`: Message queue handle for the Session Controller.
- `_usart1Mutex`: Mutex for UART communication.
- `_enabled`: Tracks whether the task is enabled.

#### Private Methods
- `bool InitOpticalSensor()`: Initializes the optical sensor.
- `bool InitForceSensorADC()`: Initializes the force sensor ADC.
- `bool InitForceSensorADS1115()`: Initializes the force sensor ADS1115.
- `float GetForce(uint16_t raw_value)`: Converts raw ADC values to force in Newtons.
- `float GetAngularVelocity(uint32_t num_posedges, uint32_t timer_counter_value)`: Calculates angular velocity from sensor data.
- `bool HasError(child_board_task_error_data& error_data)`: Checks for errors in sensor operations.
- `HAL_StatusTypeDef HAL_UART_Transmission_WithMutex(...)`: Overloaded methods for thread-safe UART communication.

#### Public Methods
- `SensorBoardController(osMessageQueueId_t sessionControllerToSensorBoardControllerHandle, osMutexId_t usart1Mutex)`: Constructor that initializes the Sensor Board Controller with message queue and mutex handles.
- `~SensorBoardController()`: Default destructor.
- `bool Init()`: Initializes the Sensor Board Controller.
- `void Run()`: The main loop of the Sensor Board Controller task.

---

## SensorBoardController.cpp

### Overview
This file implements the `SensorBoardController` class methods. It handles initialization, sensor management, and UART communication.

### Initialization

#### `Init()`
The `Init` method performs the following steps:
1. Initializes the optical sensor if enabled (`OPTICAL_ENCODER_TASK_ENABLE`).
2. Initializes the force sensor ADC if enabled (`FORCE_SENSOR_ADC_TASK_ENABLE`).
3. Initializes the force sensor ADS1115 if enabled (`FORCE_SENSOR_ADS1115_TASK_ENABLE`).
4. Configures the UART to receive data using interrupts.

### Sensor Management

#### `InitOpticalSensor()`
Sends a command to enable the optical sensor and waits for an acknowledgment.

#### `InitForceSensorADC()`
Sends a command to enable the force sensor ADC and waits for an acknowledgment.

#### `InitForceSensorADS1115()`
Configures the ADS1115 settings and sends a command to enable it, waiting for an acknowledgment.

#### `GetForce(uint16_t raw_value)`
Converts raw ADC values to force in Newtons using predefined calibration constants.

#### `GetAngularVelocity(uint32_t num_posedges, uint32_t timer_counter_value)`
Calculates angular velocity based on the number of positive edges and timer counter values.

### UART Communication

#### `HAL_UART_Transmission_WithMutex(...)`
Overloaded methods for thread-safe UART communication. These methods wrap UART operations with mutex acquisition and release.

### Error Handling

#### `HasError(child_board_task_error_data& error_data)`
Checks for errors in sensor operations and logs them to the task error buffer.

### Main Loop

#### `Run()`
The main loop of the Sensor Board Controller task. It performs the following steps:
1. Waits for data from the UART or other triggers.
2. Processes the received data and updates the corresponding circular buffers.
3. Handles errors and logs them to the task error buffer.

---

## sensorboardcontroller_main.h

### Overview
This file declares the `sensorboardcontroller_main()` function, which is the entry point for the Sensor Board Controller task.

### `sensorboardcontroller_main()`
The entry point for the Sensor Board Controller task. It:
1. Constructs a `SensorBoardController` instance.
2. Calls `Init()` to initialize the Sensor Board Controller.
3. Calls `Run()` to start the main loop.

---

## Summary of Inter-Task Communication

```
SensorBoardController
    ├── ← Session Controller queue   (commands to enable/disable sensors)
    ├── → Circular Buffers           (optical encoder, force sensor data)
    └── → Task Error Buffer          (sensor operation errors)
```

---

## Conclusion
The Sensor Board Controller task is a critical component for managing sensor data and ensuring reliable communication with the main board. Its modular design and robust error handling make it easy to integrate and maintain in the system.