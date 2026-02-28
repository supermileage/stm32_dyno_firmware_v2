# Lumex LCD Task

## Overview
The Lumex LCD task is responsible for controlling a Lumex LCD display. It provides functionality to initialize the display, write data to it, and handle commands such as clearing the display or writing strings to specific positions.

---

## Table of Contents
1. [LumexLCD.hpp](#lumexlcdhpp)
2. [LumexLCD.cpp](#lumexlcdcpp)
3. [lumexlcd_main.h](#lumexlcd_mainh)

---

## LumexLCD.hpp

### Overview
This file defines the `LumexLCD` class, which encapsulates the functionality of the Lumex LCD task. It includes methods for initialization, display operations, and error handling.

### Class Members

#### Private Members
- `_task_error_buffer_writer`: Writes error data to a circular buffer.
- `_fromSCqHandle`: Message queue handle for the Session Controller.

#### Private Methods
- `bool StartTimer(uint8_t microseconds)`: Starts a hardware timer for precise delays.
- `bool SendByte(uint8_t byte)`: Sends a byte to the LCD by toggling GPIO pins.
- `bool WriteData(uint8_t data)`: Sends data to the LCD for display.
- `bool WriteCommand(uint8_t command)`: Sends commands to configure the LCD.
- `bool ClearDisplay()`: Clears the LCD screen.
- `bool SetCursor(uint8_t row, uint8_t column)`: Moves the cursor to a specific row and column.
- `bool DisplayChar(uint8_t row, uint8_t column, uint8_t character)`: Displays a single character at a specific position.
- `bool DisplayString(uint8_t row, uint8_t column, const char* string, size_t size)`: Displays a string starting at a specific position.
- `bool ToggleBlink(bool enable)`: Enables or disables cursor blinking.

#### Public Methods
- `LumexLCD(osMessageQueueId_t sessionControllerToLumexLcdHandle)`: Constructor that initializes the Lumex LCD with a message queue handle.
- `~LumexLCD()`: Default destructor.
- `bool Init()`: Initializes the Lumex LCD.
- `void Run()`: The main loop of the Lumex LCD task.

---

## LumexLCD.cpp

### Overview
This file implements the `LumexLCD` class methods. It handles initialization, display operations, and error handling.

### Initialization

#### `Init()`
The `Init` method performs the following steps:
1. Configures the LCD to command mode.
2. Sends the initialization sequence to set the LCD to 8-bit mode, 2-line display, and 5x8 font.
3. Turns on the display without a cursor or blinking.
4. Clears the display.

### Display Operations

#### `StartTimer(uint8_t microseconds)`
Starts a hardware timer to control the timing of LCD operations.

#### `SendByte(uint8_t byte)`
Sends a byte to the LCD by toggling GPIO pins corresponding to the data lines. Uses the timer to control the enable pin timing.

#### `WriteData(uint8_t data)`
Sends data to the LCD for display.

#### `WriteCommand(uint8_t command)`
Sends commands to configure the LCD.

#### `ClearDisplay()`
Clears the LCD screen.

#### `SetCursor(uint8_t row, uint8_t column)`
Moves the cursor to a specific row and column.

#### `DisplayChar(uint8_t row, uint8_t column, uint8_t character)`
Displays a single character at a specific position.

#### `DisplayString(uint8_t row, uint8_t column, const char* string, size_t size)`
Displays a string starting at a specific position.

#### `ToggleBlink(bool enable)`
Enables or disables cursor blinking.

### Error Handling

#### `StallIfIsBufferFull(bool bufferFull)`
Logs errors encountered during timer operations to a circular buffer using the `task_error_data` structure.

### Main Loop

#### `Run()`
The main loop of the Lumex LCD task. It performs the following steps:
1. Waits for messages from the Session Controller queue.
2. Processes commands such as:
   - `CLEAR_DISPLAY`: Clears the LCD display.
   - `WRITE_TO_DISPLAY`: Writes a string to a specific row and column on the display.
3. Drains any remaining messages in the queue to ensure all commands are processed.
4. Delays the task for a predefined period (`LCD_TASK_OSDELAY`).

---

## lumexlcd_main.h

### Overview
This file declares the `lumex_lcd_main()` function, which is the entry point for the Lumex LCD task.

### `lumex_lcd_main()`
The entry point for the Lumex LCD task. It:
1. Constructs a `LumexLCD` instance.
2. Calls `Init()` to initialize the Lumex LCD.
3. Calls `Run()` to start the main loop.

---

## Summary of Inter-Task Communication

```
LumexLCD
    ├── ← Session Controller queue   (commands to clear or write to the display)
    └── → Task Error Buffer          (errors encountered during LCD operations)
```

---

## Conclusion
The Lumex LCD task provides robust control of the Lumex LCD display, enabling clear and efficient communication with the user. Its modular design and use of message queues make it easy to integrate with other tasks in the system.