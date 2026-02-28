# Session Controller Task

## Overview
The Session Controller is the central orchestration task of the dyno firmware. It manages the overall system state, coordinates all other tasks (BPM, PID, USB, SD, LCD, Sensor Board Controller), processes user inputs through a Finite State Machine (FSM), and computes physical quantities such as torque and power from sensor data.

---

## Table of Contents
1. [Input Manager Interrupts](#input-manager-interrupts)
2. [Finite State Machine (FSM)](#finite-state-machine-fsm)
3. [Session Controller](#session-controller)

---

## Input Manager Interrupts

### Files
- `Core/Inc/Tasks/SessionController/input_manager_interrupts.h`
- `Core/Src/Tasks/SessionController/input_manager_interrupts.c`

### Overview
The Input Manager Interrupts module is a C module responsible for capturing hardware button and rotary encoder inputs via GPIO interrupts. It decouples the interrupt service routines from the FSM logic by writing to a shared circular buffer that the FSM reads from in the main task loop. This design avoids doing heavy processing inside ISRs.

### Data Structures

#### `button_opcode`
An enum identifying which input was triggered:
| Value | Description |
|---|---|
| `ROT_EN_TICKS` | Rotary encoder tick (positive or negative direction) |
| `ROT_EN_SW` | Rotary encoder switch (push button) |
| `BTN_BACK` | Back button |
| `BTN_SELECT` | Select button |
| `BTN_BRAKE` | Brake button |

#### `button_press_data`
A struct stored in the circular buffer for each input event:
- `opcode`: Which button was pressed.
- `positive`: A boolean whose meaning depends on the opcode:
  - `ROT_EN_TICKS`: `true` = clockwise tick, `false` = counter-clockwise tick.
  - `BTN_BRAKE`: `true` = brake pressed, `false` = brake released.
  - All other opcodes: not used.

### Circular Buffer
The module maintains a statically allocated circular buffer `button_press_circular_buffer[USER_INPUT_CIRCULAR_BUFFER_SIZE]`. A shared index `interrupt_input_data_index` tracks the write position. The FSM tracks its own read position (`_fsmInputDataIndex`) and advances it independently, consuming events at its own pace.

> **Note:** The circular buffer is written from interrupt context and read from task context. IRQ masking is currently commented out — care must be taken if re-entrancy between interrupt handlers becomes a concern.

### Interrupt Handlers

#### `register_rotary_encoder_input()`
Called on a rising edge of `ROT_EN_A`. Reads the state of `ROT_EN_B` to determine rotation direction:
- `ROT_EN_B` high → positive (clockwise) tick.
- `ROT_EN_B` low → negative (counter-clockwise) tick.
Calls `add_to_circular_buffer(ROT_EN_TICKS, positive)`.

#### `register_rotary_encoder_sw_input()`
Called on a GPIO edge of `ROT_EN_SW`. Only registers the event when the pin state is HIGH (button release). Calls `add_to_circular_buffer(ROT_EN_SW, false)`.

#### `register_button_back_input()`
Called on a GPIO edge of `BTN_BACK`.
- PIN_RESET: Turns on the back LED (button held).
- PIN_SET: Turns off the back LED and registers the event.

#### `register_button_select_input()`
Called on a GPIO edge of `BTN_SELECT`.
- PIN_RESET: Turns on the select LED (button held).
- PIN_SET: Turns off the select LED and registers the event.

#### `register_button_brake_input()`
Called on a GPIO edge of `BTN_BRAKE`.
- PIN_SET: Turns on the brake LED and registers the event as `positive = true`.
- PIN_RESET: Turns off the brake LED and registers the event as `positive = false`.

### Helper Functions

#### `add_to_circular_buffer(button_opcode opcode, bool positive)`
Writes a `button_press_data` entry to the circular buffer at the current `interrupt_input_data_index` and increments the index (modulo buffer size).

#### `get_circular_buffer_data(uint32_t index)`
Returns a pointer to the `button_press_data` at the specified index in the circular buffer.

---

## Finite State Machine (FSM)

### Files
- `Core/Inc/Tasks/SessionController/FiniteStateMachine.hpp`
- `Core/Src/Tasks/SessionController/FiniteStateMachine.cpp`

### Overview
The FSM class encodes all user-facing behavior of the dyno. It tracks system configuration (USB/SD logging, PID enable, desired RPM), manages navigation through a menu/settings UI on the LCD, and transitions between the three top-level operating modes: Idle, Settings Menu, and In Session. It also acts as the LCD display interface, sending formatted strings to the Lumex LCD task.

### State Structure

The FSM state is represented by the `State` struct which holds three orthogonal sub-states:

#### `MainDynoState`
The top-level operating mode:
| State | Description |
|---|---|
| `IDLE` | System is idle, waiting for user to navigate to settings or start a session |
| `SETTINGS_MENU` | User is navigating the settings menu |
| `IN_SESSION` | A dyno session is in progress; sensor data is being collected and displayed |

#### `SettingsState`
Tracks the current position within the settings menu. Each setting has two sub-states — a "Displayed" state (option shown, not yet editing) and an "Edit" state (user is actively modifying the value):
| State | Description |
|---|---|
| `USB_LOGGING_OPTION_DISPLAYED` | USB logging option is shown |
| `USB_LOGGING_OPTION_EDIT` | User is editing USB logging toggle |
| `SD_LOGGING_OPTION_DISPLAYED` | SD logging option is shown |
| `SD_LOGGING_OPTION_EDIT` | User is editing SD logging toggle |
| `PID_ENABLE_DISPLAYED` | PID enable option is shown |
| `PID_ENABLE_EDIT` | User is editing PID enable toggle |
| `PID_DESIRED_RPM_DISPLAYED` | Desired RPM option is shown |
| `PID_DESIRED_RPM_EDIT` | User is editing the desired RPM value |

#### `DesiredRpmUnitsState`
Tracks which digit place the user is currently editing when setting the desired RPM:
| State | Increment |
|---|---|
| `TEN_THOUSAND` | ±10,000 RPM |
| `THOUSAND` | ±1,000 RPM |
| `HUNDRED` | ±100 RPM |
| `TEN` | ±10 RPM |
| `ONE` | ±1 RPM |

Each rotary encoder tick increments or decrements the desired RPM by the amount corresponding to the current `DesiredRpmUnitsState`. Pressing the rotary encoder switch advances to the next digit place.

### Input Handling

#### `HandleUserInputs()`
Called every iteration of the Session Controller's main loop. Processes all pending entries in the Input Manager circular buffer from `_fsmInputDataIndex` up to `interrupt_input_data_index`, dispatching each event to the appropriate handler:
- `ROT_EN_TICKS` → `HandleRotaryEncoderInput(positive)`
- `ROT_EN_SW` → `HandleRotaryEncoderSwInput()`
- `BTN_BACK` → `HandleButtonBackInput()`
- `BTN_SELECT` → `HandleButtonSelectInput()`
- `BTN_BRAKE` → `HandleButtonBrakeInput(positive)`

#### `HandleRotaryEncoderInput(bool positiveTick)`
Behavior depends on `mainState`:
- **IDLE**: No action.
- **SETTINGS_MENU**: Navigates between settings options (displayed states) or modifies the current setting (edit states). For `PID_DESIRED_RPM_EDIT`, calls `ConvertUserInputIntoDesiredRpm()`.
- **IN_SESSION**: Adjusts the manual BPM or throttle duty cycle depending on which manual mode is active.

#### `HandleRotaryEncoderSwInput()`
Advances state in the settings menu or cycles through digit places in `PID_DESIRED_RPM_EDIT`.

#### `HandleButtonBackInput()`
- In **SETTINGS_MENU**: Moves back through settings states, or exits back to IDLE from the first settings option.
- In **IN_SESSION**: Returns to IDLE and ends the session.

#### `HandleButtonSelectInput()`
- In **IDLE**: Enters the settings menu.
- In **SETTINGS_MENU**: Enters edit mode for the displayed option, or confirms and saves the current edit.
- In **IN_SESSION**: Contextual select action (e.g., toggle PID).

#### `HandleButtonBrakeInput(bool isEnabled)`
Toggles the `_inSession` flag. When the brake is pressed (enabled), a session starts; when released, the session ends.

### State Transition Methods
Each state has a corresponding private method that updates the LCD display to reflect the new state:

| Method | Displayed Content |
|---|---|
| `IdleState()` | Idle screen |
| `USBLoggingOptionDisplayedSettingsState()` | USB logging status |
| `USBLoggingOptionEditSettingsState()` | USB logging edit prompt |
| `SDLoggingOptionDisplayedSettingsState()` | SD logging status |
| `SDLoggingOptionEditSettingsState()` | SD logging edit prompt |
| `PIDOptionDisplayedSettingsState()` | PID enable status |
| `PIDOptionEditSettingsState()` | PID enable edit prompt |
| `PIDDesiredRPMOptionDisplayedSettingsState()` | Current desired RPM |
| `PIDDesiredRPMOptionEditSettingsState(bool clearDisplay)` | Desired RPM with digit edit cursor |
| `InSessionState()` | Live session display (RPM, torque, power) |

### LCD Communication
All display updates are sent via `AddToLumexLCDMessageQueue()`, which constructs a `session_controller_to_lumex_lcd` message and puts it onto the Lumex LCD queue. The parameters specify the opcode (write or clear), row, column, string content, and size.

Dedicated display helpers are also provided:
- `DisplayRpm(float rpm)`: Formats and displays RPM on the LCD.
- `DisplayTorque(float torque)`: Formats and displays torque.
- `DisplayPower(float power)`: Formats and displays power.
- `DisplayPIDEnabled()`: Displays PID status.
- `DisplayManualBPMDutyCycle()`: Displays the manual BPM duty cycle.
- `DisplayManualThrottleDutyCycle()`: Displays the manual throttle duty cycle.

### Getters
The FSM exposes the following read-only state accessors used by the Session Controller:

| Method | Returns |
|---|---|
| `GetState()` | Current `State` struct |
| `GetUSBLoggingEnabledStatus()` | Whether USB logging is enabled |
| `GetSDLoggingEnabledStatus()` | Whether SD logging is enabled |
| `GetPIDEnabledModeStatus()` | Whether PID mode is active |
| `GetPIDOptionToggleableEnabledStatus()` | Whether PID is available to toggle |
| `GetManualBpmModeStatus()` | Whether manual BPM mode is active |
| `GetManualThrottleModeStatus()` | Whether manual throttle mode is active |
| `GetInSessionStatus()` | Whether a session is currently running |
| `GetDesiredBpmDutyCycle()` | Current manual BPM duty cycle target |
| `GetDesiredThrottleDutyCycle()` | Current manual throttle duty cycle target |
| `GetDesiredRpm()` | Target RPM (for PID) |
| `GetDesiredAngularVelocity()` | Target angular velocity (rad/s) derived from desired RPM |

---

## Session Controller

### Files
- `Core/Inc/Tasks/SessionController/SessionController.hpp`
- `Core/Inc/Tasks/SessionController/sessioncontroller_main.h`
- `Core/Src/Tasks/SessionController/SessionController.cpp`

### Overview
The `SessionController` class is the main coordination layer of the firmware. It is responsible for:
- Initializing the timestamp timer.
- Validating all inter-task message queue handles.
- Running the main control loop, which polls the FSM, dispatches commands to other tasks, reads sensor buffer data, and computes physical quantities.

### Initialization

#### Queue Validation: `CheckTaskQueuesValid()`
Before running, the Session Controller verifies that all required message queue handles in `session_controller_os_task_queues` are non-null. Which queues are checked is controlled by compile-time feature flags:
- `USB_CONTROLLER_TASK_ENABLE`
- `SD_CONTROLLER_TASK_ENABLE`
- `BPM_CONTROLLER_TASK_ENABLE`
- `PID_CONTROLLER_TASK_ENABLE`
- `LUMEX_LCD_TASK_ENABLE`
- `FORCE_SENSOR_ADS1115_TASK_ENABLE / FORCE_SENSOR_ADC_TASK_ENABLE / OPTICAL_ENCODER_TASK_ENABLE`

If any required handle is null, an error is logged and `Init()` returns `false`, causing the task to suspend.

#### `Init()`
1. Starts the hardware timestamp timer via `start_timestamp_timer()`.
2. Calls `CheckTaskQueuesValid()`.

### Task Queue Structure: `session_controller_os_task_queues`
Defined in `sessioncontroller_main.h`, this struct holds all outgoing queue handles the Session Controller uses to communicate with other tasks:

| Field | Target Task |
|---|---|
| `usb_controller` | USB Controller |
| `sd_controller` | SD Controller |
| `bpm_controller` | BPM Controller |
| `pid_controller` | PID Controller (outgoing commands) |
| `pid_controller_ack` | PID Controller (incoming acknowledgments) |
| `lumex_lcd` | Lumex LCD |
| `sensor_board_controller` | Sensor Board Controller |

### Main Loop: `Run()`

The `Run()` method operates in an infinite loop with the following stages executed every iteration:

#### 1. Handle User Inputs
Calls `_fsm.HandleUserInputs()` to process any pending button or encoder events from the interrupt circular buffer.

#### 2. USB Logging Control
Reads `_fsm.GetUSBLoggingEnabledStatus()`. If the status has changed since the last iteration (`XOR` with `_prevUSBLoggingEnabled`), sends the new enable state to the USB Controller queue. Uses edge detection to avoid redundant queue messages.

#### 3. SD Logging Control
Same edge-detection pattern as USB logging. Sends to the SD Controller queue on state change.

#### 4. Session Lifecycle Management
Detects rising and falling edges on `_fsm.GetInSessionStatus()`:

**Rising Edge (Session Start):**
- Enables the Sensor Board Controller.
- Resets the LCD display to show zeroed RPM, torque, and power.
- Sends the appropriate BPM command based on whether PID toggleable mode is enabled.

**Falling Edge (Session End):**
- Disables sensors and motor control.
- Sends stop commands to the BPM and Sensor Board Controller.

If not in session, the loop delays (`SESSIONCONTROLLER_TASK_OSDELAY`) and continues, skipping all session-only logic below.

#### 5. PID Enable Control
If PID enabled status has changed, constructs a `session_controller_to_pid_controller` message with the new enable state and the current desired angular velocity from the FSM, then puts it on the PID Controller queue.

#### 6. PID Acknowledgment Handling
After sending a PID enable/disable command, the Session Controller waits for an acknowledgment from the PID Controller via `pid_controller_ack`. Once received:
- If PID is now enabled: sends `READ_FROM_PID` to the BPM Controller so it begins consuming duty cycle values from the PID queue.
- Handles toggling behavior for the PID option if `PIDOptionToggleableEnabled` is set.

#### 7. Manual Throttle / BPM Control
If PID toggleable mode is not active, the Session Controller handles manual control:
- **Manual Throttle Mode**: Reads the desired throttle duty cycle from the FSM and sends a `START_PWM` command to the throttle (BPM) queue if it has changed.
- **Manual BPM Mode**: Reads the desired BPM duty cycle from the FSM and sends a `START_PWM` command to the BPM queue if it has changed.
- **Neither**: Sends `STOP_PWM` to the BPM Controller.

#### 8. Sensor Data Consumption
Drains the latest data from the circular buffers for both sensors:
- `_forcesensor_buffer_reader`: Reads all available `forcesensor_output_data` entries, keeping the latest.
- `_optical_encoder_buffer_reader`: Reads all available `optical_encoder_output_data` entries, keeping the latest.

#### 9. Physical Quantity Computation
Computes:
- **Torque**: `CalculateTorque(angularAcceleration, force, angularVelocity)`
- **Power**: `CalculatePower(torque, angularVelocity)`

These are only sent to the display if the values have changed since the last iteration.

#### 10. LCD Update
If `angularVelocity` has changed:
- Calls `_fsm.DisplayRpm(angular_velocity)`.
- If force has also changed, calls `_fsm.DisplayTorque()` and `_fsm.DisplayPower()`.

### Physical Calculations

#### `CalculateTorque(angularAcceleration, force, angularVelocity)`
$$\tau = I \cdot \alpha + F \cdot d + \tau_{losses}$$
Where:
- $I$ = `MOMENT_OF_INERTIA_KG_M2`
- $\alpha$ = `angularAcceleration`
- $F$ = measured force in Newtons
- $d$ = `DISTANCE_FROM_FORCE_SENSOR_TO_CENTER_OF_SHAFT_M`
- $\tau_{losses}$ = `CalculateMechanicalLosses()` (currently returns 0; reserved for future calibration)

#### `CalculatePower(torque, angularVelocity)`
$$P = \tau \cdot \omega$$

#### `CalculateMechanicalLosses(angularAcceleration, angularVelocity)`
Currently returns `0`. Intended to model friction and drivetrain losses as a function of angular velocity and acceleration.

### Error Handling
Errors in initialization are logged to the `task_error_circular_buffer` with the task ID `TASK_ID_SESSION_CONTROLLER`. On a fatal error, `Init()` returns `false` and the task suspends itself via `osThreadSuspend()`.

### Entry Point
`sessioncontroller_main(session_controller_os_task_queues* task_queues)` is the C-linkage entry point called by FreeRTOS on task startup. It constructs the `SessionController`, runs `Init()`, and then calls `Run()`.

---

## Summary of Inter-Task Communication

```
SessionController
    ├── → USB Controller queue       (enable/disable logging)
    ├── → SD Controller queue        (enable/disable logging)
    ├── → BPM Controller queue       (START_PWM / STOP_PWM / READ_FROM_PID)
    ├── → PID Controller queue       (enable + desired angular velocity)
    ├── ← PID Controller ack queue   (acknowledgment of enable/disable)
    ├── → Lumex LCD queue            (display strings / clear via FSM)
    └── → Sensor Board Controller queue (enable/disable sensors)
```
