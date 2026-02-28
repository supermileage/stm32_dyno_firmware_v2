# USB Controller Task

## Overview
The USB Controller task is responsible for managing USB communication between the firmware and an external application. It collects data from various tasks, formats it into a structured message format, and transmits it over USB. The task also handles incoming acknowledgments from the external application to ensure synchronization.

---

## Table of Contents
1. [USBController.hpp](#usbcontrollerhpp)
2. [USBController.cpp](#usbcontrollercpp)
3. [usbcontroller_main.h](#usbcontroller_mainh)

---

## USBController.hpp

### Overview
This file defines the `USBController` class, which encapsulates the functionality of the USB Controller task. It includes methods for initialization, data processing, and USB communication.

### Class Members

#### Private Members
- `_task_errors_buffer_reader`: Reads task error data from the circular buffer.
- `_buffer_reader_optical_encoder`: Reads optical encoder data from the circular buffer.
- `_buffer_reader_forcesensor`: Reads force sensor data from the circular buffer.
- `_buffer_reader_bpm`: Reads BPM data from the circular buffer.
- `_taskMonitorToUsbControllerHandle`: Message queue handle for receiving task monitor data.
- `_sessionControllerToUsbController`: Message queue handle for receiving session controller commands.
- `_txBuffer`: Transmit buffer for USB data.
- `_txBufferIndex`: Current write index in the transmit buffer.

#### Private Methods
- `StallIfIsBufferFull(bool bufferFull)`: Blocks until there is space in the transmit buffer.
- `bool IsBufferFull(std::size_t msgSize)`: Checks if the transmit buffer has enough space for a new message.
- `void ProcessErrorsAndWarnings()`: Reads and processes task error messages.
- `void ReceiveAppAck()`: Waits for an acknowledgment from the external application before starting data transmission.
- `template <typename T> void AddToBuffer(T* msg, size_t msgSize)`: Adds a message to the transmit buffer.
- `template <typename T> void ProcessTaskData(CircularBufferReader<T>& bufferReader, task_ids_t taskId)`: Processes data from a circular buffer and adds it to the transmit buffer.
- `template <typename T> void ProcessTaskData(osMessageQueueId_t msgqHandle, task_ids_t taskId)`: Processes data from a message queue and adds it to the transmit buffer.

#### Public Methods
- `USBController(osMessageQueueId_t sessionControllerToUsbController, osMessageQueueId_t taskMonitorToUsbControllerHandle)`: Constructor that initializes the USB Controller with message queue handles.
- `~USBController()`: Default destructor.
- `bool Init()`: Initializes the USB Controller.
- `void Run()`: The main loop of the USB Controller task, which processes and transmits data.
- `void MockMessages(const bool forever = true)`: Generates mock data for testing USB communication.

---

## USBController.cpp

### Overview
This file implements the `USBController` class methods. It handles initialization, data processing, and USB communication.

### Initialization

#### `Init()`
Currently, this method always returns `true`. Future enhancements may include additional initialization logic.

### Data Processing

#### `Run()`
The main loop of the USB Controller task. It performs the following steps:
1. Waits for an acknowledgment from the external application using `ReceiveAppAck()`.
2. Waits for a command from the Session Controller to enable or disable USB logging.
3. If USB logging is enabled, processes data from the following sources:
   - **Optical Encoder**: Reads data from `_buffer_reader_optical_encoder` and adds it to the transmit buffer.
   - **Force Sensor**: Reads data from `_buffer_reader_forcesensor` and adds it to the transmit buffer.
   - **BPM Controller**: Reads data from `_buffer_reader_bpm` and adds it to the transmit buffer.
   - **Task Monitor**: Reads data from `_taskMonitorToUsbControllerHandle` and adds it to the transmit buffer.
   - **Task Errors**: Reads error messages from `_task_errors_buffer_reader` and adds them to the transmit buffer.
4. Transmits the contents of the transmit buffer over USB using `CDC_Transmit_FS()`.
5. Delays for `USB_TASK_OSDELAY` before the next iteration.

#### `MockMessages(const bool forever)`
Generates mock data for testing USB communication. This method simulates data from the optical encoder, force sensor, BPM controller, and task monitor, as well as task error messages. It is used when `DEBUG_USB_CONTROLLER_MOCK_MESSAGES` is enabled.

### USB Communication

#### `ReceiveAppAck()`
Waits for an acknowledgment (`"OK"`) from the external application before starting data transmission. This ensures that the application is ready to receive data.

#### `StallIfIsBufferFull(bool bufferFull)`
Blocks until there is enough space in the transmit buffer to add a new message.

#### `bool IsBufferFull(std::size_t msgSize)`
Checks if the transmit buffer has enough space for a new message, including the message header.

#### `ProcessErrorsAndWarnings()`
Reads task error messages from `_task_errors_buffer_reader` and adds them to the transmit buffer.

---

## usbcontroller_main.h

### Overview
This file declares the `usbcontroller_main()` function, which is the entry point for the USB Controller task.

### `usbcontroller_main()`
The entry point for the USB Controller task. It:
1. Constructs a `USBController` instance.
2. Calls `Init()` to initialize the USB Controller.
3. If `DEBUG_USB_CONTROLLER_MOCK_MESSAGES` is enabled, calls `MockMessages()` to generate mock data.
4. Otherwise, calls `Run()` to start the main loop.

---

## Summary of Inter-Task Communication

```
USBController
    ├── ← Session Controller queue   (enable/disable USB logging)
    ├── ← Task Monitor queue         (task state and stack usage data)
    ├── ← Circular Buffers           (optical encoder, force sensor, BPM data)
    ├── ← Task Error Buffer          (task error messages)
    └── → USB Application            (formatted data messages)
```

---

## Conclusion
The USB Controller task is a critical component for data logging and debugging. Its modular design and use of circular buffers and message queues make it efficient and easy to integrate with other tasks. The ability to generate mock data further enhances its utility for testing and development.