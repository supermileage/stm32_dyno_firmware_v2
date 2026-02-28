# Task Monitor Task

## Overview
The Task Monitor is responsible for monitoring the state and resource usage of all FreeRTOS tasks in the system. It periodically collects task state, stack usage, and other diagnostic data, and sends this information to the USB Controller for logging or debugging purposes. The Task Monitor also validates the thread IDs of all tasks during initialization to ensure proper operation.

---

## Table of Contents
1. [TaskMonitor.hpp](#taskmonitorhpp)
2. [TaskMonitor.cpp](#taskmonitorcpp)
3. [taskmonitor_main.h](#taskmonitor_mainh)

---

## TaskMonitor.hpp

### Overview
This file defines the `TaskMonitor` class, which encapsulates the functionality of the Task Monitor task. It includes methods for initialization, task monitoring, and data transmission.

### Class Members

#### Private Members
- `_task_error_buffer_writer`: A `CircularBufferWriter` for logging task errors.
- `_osThreadIdPtrs`: A pointer to a `taskmonitor_osthreadids` struct containing thread IDs of all monitored tasks.
- `_taskMonitorToUsbControllerHandle`: The message queue handle for sending task data to the USB Controller.

#### Private Methods
- `GetTaskDataAndSendToUsbController(task_ids_t task_id, osThreadId_t thread_id)`: Collects task state and stack usage for a given task and sends the data to the USB Controller.

#### Public Methods
- `TaskMonitor(taskmonitor_osthreadids* osthreadid_ptrs, osMessageQueueId_t taskMonitorToUsbControllerHandle)`: Constructor that initializes the Task Monitor with thread IDs and the USB Controller queue handle.
- `~TaskMonitor()`: Default destructor.
- `bool Init()`: Validates the thread ID pointers and logs an error if any are invalid.
- `void Run()`: The main loop of the Task Monitor task, which periodically collects and sends task data.

---

## TaskMonitor.cpp

### Overview
This file implements the `TaskMonitor` class methods. It handles initialization, task monitoring, and communication with the USB Controller.

### Initialization

#### `Init()`
Validates the thread ID pointers in `_osThreadIdPtrs`. If any required pointer is null, logs an error to the `task_error_circular_buffer` and returns `false`. The validation is controlled by compile-time feature flags:
- `SESSION_CONTROLLER_TASK_ENABLE`
- `USB_CONTROLLER_TASK_ENABLE`
- `SD_CONTROLLER_TASK_ENABLE`
- `SENSOR_BOARD_CONTROLLER_TASK_ENABLE`
- `BPM_CONTROLLER_TASK_ENABLE`
- `PID_CONTROLLER_TASK_ENABLE`
- `LUMEX_LCD_TASK_ENABLE`

### Task Monitoring

#### `Run()`
The main loop of the Task Monitor task. For each enabled task, calls `GetTaskDataAndSendToUsbController()` to collect and send task data. Also monitors its own task state. Delays for `TASK_MONITOR_TASK_OSDELAY` between iterations.

#### `GetTaskDataAndSendToUsbController(task_ids_t task_id, osThreadId_t thread_id)`
1. Collects the following data for the specified task:
   - `timestamp`: Current system timestamp.
   - `task_id`: The ID of the task being monitored.
   - `task_state`: The current state of the task (e.g., `Ready`, `Running`, `Blocked`).
   - `free_bytes`: The remaining stack space for the task, in bytes.
2. Sends the data to the USB Controller queue as a `task_monitor_output_data` message.

### Stack Overflow Hook
If `configCHECK_FOR_STACK_OVERFLOW` is enabled, the `vApplicationStackOverflowHook()` function halts the system in the event of a stack overflow.

---

## taskmonitor_main.h

### Overview
This file declares the `taskmonitor_main()` function and the `taskmonitor_osthreadids` struct, which holds the thread IDs of all monitored tasks.

### `taskmonitor_osthreadids`
A struct containing thread IDs for all tasks monitored by the Task Monitor:

| Field | Description |
|---|---|
| `session_controller` | Thread ID of the Session Controller task |
| `usb_controller` | Thread ID of the USB Controller task |
| `sd_controller` | Thread ID of the SD Controller task |
| `sensor_board_controller` | Thread ID of the Sensor Board Controller task |
| `bpm_controller` | Thread ID of the BPM Controller task |
| `pid_controller` | Thread ID of the PID Controller task |
| `pid_controller_ack` | Thread ID of the PID Controller acknowledgment task |
| `lumex_lcd` | Thread ID of the Lumex LCD task |

### `taskmonitor_main()`
The entry point for the Task Monitor task. It:
1. Constructs a `TaskMonitor` instance.
2. Calls `Init()` to validate thread IDs.
3. If initialization succeeds, calls `Run()` to start the main loop.

---

## Summary of Inter-Task Communication

```
TaskMonitor
    ├── → USB Controller queue       (task state and stack usage data)
    └── → Task Error Buffer          (initialization errors)
```

---

## Conclusion
The Task Monitor task provides critical diagnostic data for debugging and monitoring the health of the system. Its modular design and use of compile-time flags make it easy to integrate and customize for different system configurations.