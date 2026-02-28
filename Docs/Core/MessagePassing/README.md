# Message Passing Overview

This document provides an overview of the message-passing headers and source files in this repository. It explains their purpose, usage, and how they interact with the system.

## Headers

### `messages_private.h`
- **Purpose**: Contains definitions and structures for message passing that are specific to this repository.
- **Scope**: These messages are only used within this repository and are not shared with upper-level repositories.
- **Usage**: Use these definitions for internal communication between tasks and modules within this firmware.

### `messages_public.h`
- **Purpose**: Contains definitions and structures for message passing that are shared between this repository and upper-level repositories (e.g., the software repository).
- **Scope**: These messages are used for communication between this firmware and external systems.
- **Usage**: Use these definitions for any messages that need to be shared or understood by external systems.

## Queue Helper Functions

### `EmptyQueue`
- **Purpose**: Clears all messages from a CMSIS-RTOS2 message queue.
- **Prototype**: `void EmptyQueue(osMessageQueueId_t qHandle, size_t itemSize);`
- **Parameters**:
  - `qHandle`: Handle of the message queue to clear.
  - `itemSize`: Size of one item in the queue (in bytes).
- **Usage**:
  ```c
  EmptyQueue(myQueueHandle, sizeof(myMessageType));
  ```
- **Behavior**: Removes all messages from the queue and discards them. Useful for resetting a queue.

### `GetLatestFromQueue`
- **Purpose**: Retrieves the latest message from a CMSIS-RTOS2 message queue, discarding older messages.
- **Prototype**: `bool GetLatestFromQueue(osMessageQueueId_t queueHandle, void* latestData, size_t itemSize, uint32_t timeout);`
- **Parameters**:
  - `queueHandle`: Handle of the message queue to read from.
  - `latestData`: Pointer to memory where the latest message will be stored.
  - `itemSize`: Size of one item in the queue (in bytes).
  - `timeout`: Timeout in ticks (use `osWaitForever` for blocking behavior).
- **Usage**:
  ```c
  myMessageType latestMessage;
  if (GetLatestFromQueue(myQueueHandle, &latestMessage, sizeof(myMessageType), osWaitForever)) {
      // Process the latest message
  }
  ```
- **Behavior**: Retrieves the most recent message from the queue, discarding older messages. Returns `true` if a message was retrieved, `false` otherwise.

## Circular Buffers

### `circular_buffers.c`
- **Purpose**: Contains the definitions for all circular buffer arrays and their writer indexes.
- **Contents**:
  - Circular buffer arrays for storing data such as optical encoder output, force sensor output, BPM data, and task errors.
  - Writer indexes for each circular buffer.
- **Usage**:
  - Use the circular buffer arrays to store data temporarily before processing or transmitting.
  - Use the writer indexes to track the current write position in each buffer.
- **Example**:
  ```c
  optical_encoder_output_data newData = { .timestamp = get_timestamp(), .angular_velocity = 1.23f };
  optical_encoder_circular_buffer[optical_encoder_circular_buffer_index_writer] = newData;
  optical_encoder_circular_buffer_index_writer = (optical_encoder_circular_buffer_index_writer + 1) % OPTICAL_ENCODER_CIRCULAR_BUFFER_SIZE;
  ```

## Summary
- Use `messages_private.h` for internal message definitions.
- Use `messages_public.h` for shared message definitions.
- Use `EmptyQueue` and `GetLatestFromQueue` to manage CMSIS-RTOS2 message queues effectively.
- Use the circular buffers in `circular_buffers.c` for temporary data storage and processing.