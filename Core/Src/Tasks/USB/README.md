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

A full sequence of every message — handshake, streaming, and command routing (including the
routed "non-posted" ack) — is in [`usb_message_flow.puml`](usb_message_flow.puml).

## Flow (`Run()`)
Each iteration:
1. `ProcessIncomingFrames()` — pull + route CRC-validated host frames (always, so the host can
   handshake/configure regardless of logging state).
2. `ProcessCompletions()` — relay applied-command acks as `USB_MSG_RESPONSE`.
3. Pick up the SessionController's USB logging enable/disable.
4. **Handshake gate:** while `!_appReady`, `AnnounceReadyIfDue()` emits a `usb_device_ready_event`
   (`USB_MSG_EVENT`, `TASK_OFFSET_USB_CONTROLLER`) about every `DEVICE_READY_ANNOUNCE_MS`.
5. Once `_appReady && enableUSB`, frame and append the streams:
   - `optical_encoder_output_data` (`USB_MSG_STREAM`, `TASK_OFFSET_OPTICAL_ENCODER`)
   - `forcesensor_output_data` (`USB_MSG_STREAM`, active force-sensor offset)
   - `bpm_output_data`, `task_monitor_output_data`
   - errors via `ProcessErrorsAndWarnings()`.
6. `CDC_Transmit_FS(_txBuffer, …)`; delay `USB_TASK_OSDELAY`.

## Handshake (device-announced)
The firmware streams nothing until the host acknowledges it. While `_appReady` is false the
device repeatedly announces `usb_device_ready_event{ USB_PROTOCOL_VERSION }`; the host answers
with a framed `USB_CMD_ACK` whose body is its own `USB_PROTOCOL_VERSION`. `HandleUsbLocalCommand`
sets `_appReady` and replies `USB_RSP_OK` when the versions match, or `USB_RSP_VERSION_MISMATCH`
(and keeps announcing) when they differ — so a host built against a stale schema is rejected at
the link instead of silently mis-decoding the stream. `MockMessages()` gates on the same
handshake via `WaitForHandshake()`.

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
