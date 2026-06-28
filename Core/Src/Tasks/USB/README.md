---
module: USB
summary: Serializes all data + error streams into framed USB-CDC messages to the PC.
code:
  - Core/Src/Tasks/USB/USBController.cpp
  - Core/Inc/Tasks/USB/USBController.hpp
  - Core/Inc/Tasks/USB/usbcontroller_main.h
entry_point: usbcontroller_main()
task_offset: TASK_OFFSET_USB_CONTROLLER
consumes: [optical_encoder/forcesensor/bpm/task_error circular buffers, taskMonitor queue, SessionController enable]
produces: [USB CDC stream to PC (CDC_Transmit_FS)]
related: [MessagePassing, TaskMonitor, SessionController]
---

# USB — host data link

Drains the data/error streams, prefixes each with a `usb_msg_header_t`, and transmits
over USB CDC. Each record = header + payload (see the wire protocol in [[MessagePassing]]).

## Flow (`Run()`)
1. `ReceiveAppAck()` — wait for the PC to send `"OK"` before streaming.
2. Wait for SessionController to enable USB logging.
3. While enabled, frame and append:
   - `optical_encoder_output_data` (`USB_MSG_STREAM`, `TASK_OFFSET_OPTICAL_ENCODER`)
   - `forcesensor_output_data` (`USB_MSG_STREAM`, active force-sensor offset)
   - `bpm_output_data`, `task_monitor_output_data`
   - errors via `ProcessErrorsAndWarnings()`.
4. `CDC_Transmit_FS(_txBuffer, …)`; delay `USB_TASK_OSDELAY`.

## Error/warning framing
`ProcessErrorsAndWarnings()` reads `task_error_data` from `task_error_circular_buffer` and sets
the header from the packed code: `msg_type = (error_code & WARNING_FLAG) ? USB_MSG_WARNING : USB_MSG_ERROR`,
`task_offset = error_code & TASK_OFFSET_MASK`. (Encoding: [[MessagePassing]].)

## Helpers / config
- `ProcessTaskData<T>(reader|queue, task_offset)`, `AddToBuffer<T>`, `IsBufferFull` / `StallIfIsBufferFull`.
- `ACTIVE_FORCE_SENSOR_TASK_OFFSET` selects ADS1115 vs ADC offset for the shared force stream.
- `MockMessages()` emits synthetic data when `DEBUG_USB_CONTROLLER_MOCK_MESSAGES` is set.
- `USB_TX_BUFFER_SIZE`, `USB_TASK_OSDELAY` (config.h).

## Related
[[MessagePassing]] · [[TaskMonitor]] · [[SessionController]]
