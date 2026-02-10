# Timekeeping with `timestamps.c`

## Overview
The `timestamps.c` file provides functionality for generating timestamp values, which are essential for logging and tracking when specific data is collected during the operation of the system.

## Key Functionality
- **`get_timestamp()`**: This function returns a timestamp value ranging from `0` to `UINT32_MAX`. It provides a general-purpose way to keep time with high precision.

## Current Setup
- **Precision**: Each tick corresponds to `1 µs` (microsecond), ensuring high precision for timekeeping.
- **Maximum Time**: The maximum timestamp value is `UINT32_MAX`, which translates to approximately `1.2 hours` of simulation time before the timer resets to `0`.
- **Timer Start**: The timer is started by the Session Controller.

## Future Considerations
- **Overflow Handling**: Currently, the system does not handle timer overflows. In the future, an overflow value may be implemented to extend the simulation time beyond `1.2 hours`.

## Usage
This timekeeping mechanism is particularly useful for:
- Logging events with precise timestamps.
- Tracking when specific data was collected during the operation of the system.