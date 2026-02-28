# PID Controller Task

## Overview
The PID Controller task is responsible for implementing a feedback loop to control the brake PWM value. It uses data from the optical encoder to calculate the error between the desired and current angular velocity, and adjusts the brake duty cycle accordingly. The session controller determines whether this task runs or not.

---

## Table of Contents
1. [Message Queues](#message-queues)
2. [Circular Buffers](#circular-buffers)
3. [Current Functionality](#current-functionality)
4. [Future Enhancements](#future-enhancements)
5. [Example Workflow](#example-workflow)
6. [Best Practices](#best-practices)

---

## Message Queues

### Overview
The PID Controller task uses the following message queues:

- **`_sessionControllerToPidHandle`**: Receives instructions from the session controller, such as enabling/disabling the PID controller and setting the desired angular velocity.
- **`_pidControllerToSessionControllerAckHandle`**: Sends acknowledgments back to the session controller.
- **`_pidToBpmHandle`**: Sends the calculated brake duty cycle to the BPM task.

---

## Circular Buffers

### Overview
The PID Controller task interacts with the following circular buffers:

- **Optical Encoder Data**: Reads optical encoder data from `optical_encoder_circular_buffer` to calculate the current angular velocity.
- **Task Error Data**: Writes errors encountered by the PID Controller to `task_error_circular_buffer` for logging and debugging purposes.

---

## Current Functionality

### Feedback Loop
The PID Controller implements a simple feedback loop to calculate the brake PWM value based on the error between the desired and current angular velocity. The loop includes proportional, integral, and derivative terms.

### Session Controller Control
The session controller determines whether the PID Controller task is enabled or disabled. When disabled, the task clears its message queue and waits for further instructions.

---

## Future Enhancements

### Throttle Control
Currently, the PID Controller only calculates the brake PWM value. Once throttle control is added, the feedback loop will need to be updated to handle both throttle and brake outputs.

### Dual Output PID
The PID Controller will need to implement a mechanism to calculate and mix both throttle and brake outputs in a way that ensures smooth operation.

---

## Example Workflow

### Initialization
The PID Controller task initializes its state and waits for instructions from the session controller.

### Receiving Instructions
The task receives a message from the session controller, enabling the PID Controller and setting the desired angular velocity.

### Feedback Loop
1. Reads the latest optical encoder data from the circular buffer.
2. Calculates the error, derivative, and integral terms.
3. Computes the brake PWM value and sends it to the BPM task.

### Idle State
If the PID Controller is disabled, it clears its message queue and waits for further instructions.

---

## Best Practices

### Integration
- Ensure that the session controller is properly configured to enable and disable the PID Controller task as needed.
- Monitor the task error circular buffer for any issues encountered by the PID Controller.

### Future-Proofing
- Design the system to handle both throttle and brake outputs in the future.