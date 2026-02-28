# CircularBufferReader and CircularBufferWriter

## Overview
The `CircularBufferReader` and `CircularBufferWriter` classes provide a flexible and efficient way to manage circular buffers in embedded systems. These classes are implemented using C++ templates, making it easy to configure the payload type (typically a struct containing data).

## Key Features
- **Multiple Readers and Writers**: Unlike `osMessageQueue`, which supports only one reader, this implementation allows multiple readers and writers to operate on the same buffer.
- **No Heap Allocation**: Designed for embedded environments, the implementation avoids dynamic memory allocation.
- **Independent Reader Indices**: Each reader maintains its own `readerIndex`, while all readers and writers share the same `writerIndex`.
- **Critical Sections**: Critical sections are used to prevent context switching during read and write operations, ensuring data consistency.

## Limitations
- **No Overflow Protection**: The implementation does not check if a reader is `ARRAY_SIZE` elements behind the writer. It assumes that all readers are keeping up with the writer.
- **Partial Write Issue**: If the writer writes one buffer size past a reader, there is a risk of the reader accessing partially updated data due to a context switch. Critical sections solve this issue, which causes minimal performance decrease, but is better for reliability.
- **Payload Size**: It is recommended to use small data types to minimize the time spent in critical sections, allowing the RTOS and interrupts to function efficiently.

## Usage
### CircularBufferWriter
The `CircularBufferWriter` class is responsible for writing data to the buffer. It provides methods to:
- Write data to a specific index.
- Write data to the current index and increment the writer index.

### CircularBufferReader
The `CircularBufferReader` class is responsible for reading data from the buffer. It provides methods to:
- Read data from a specific index.
- Read data from the current index and increment the reader index.
- Check if new data is available.

## Example
```cpp
#include "CircularBufferReader.hpp"
#include "CircularBufferWriter.hpp"

struct Data {
    int value;
};

constexpr size_t BUFFER_SIZE = 10;
Data buffer[BUFFER_SIZE];
size_t writerIndex = 0;

CircularBufferWriter<Data> writer(buffer, &writerIndex, BUFFER_SIZE);
CircularBufferReader<Data> reader(buffer, &writerIndex, BUFFER_SIZE);

void writeData() {
    Data data = {42};
    writer.WriteElementAndIncrementIndex(data);
}

void readData() {
    Data data;
    if (reader.GetElementAndIncrementIndex(data)) {
        // Process data
    }
}
```

## Best Practices
- Ensure that all readers keep up with the writer to avoid data loss.
- Use small payloads to minimize the time spent in critical sections.
- Carefully design the system to avoid scenarios where the writer overwrites unread data.

## Array Storage
- The arrays used by the `CircularBufferReader` and `CircularBufferWriter` classes are defined in `MessagePassing/circular_buffers.c`. This file contains the buffer arrays and their corresponding writer indices.