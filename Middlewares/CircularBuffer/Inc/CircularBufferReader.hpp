#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_

#include <stdint.h>

// Templated circular buffer reader for embedded systems.
// Does NOT own the memory. Reads from external buffer.

template <typename T>
class CircularBufferReader
{
public:
    CircularBufferReader(T* buffer, uint32_t size);

    // Index management
    uint32_t GetIndex() const;
    void SetIndex(uint32_t index);

    // Element access
    T GetElement(uint32_t index) const;
    T GetElement() const;
    T GetElementAndIncrementIndex();

private:
    T* _buffer;           // external buffer memory
    uint32_t _size;       // number of elements
    uint32_t _index;      // current read index
};

// =======================
// Template Implementation
// =======================

template <typename T>
inline CircularBufferReader<T>::CircularBufferReader(T* buffer, uint32_t size)
    : _buffer(buffer), _size(size), _index(0)
{
}

template <typename T>
inline uint32_t CircularBufferReader<T>::GetIndex() const
{
    return _index;
}

template <typename T>
inline void CircularBufferReader<T>::SetIndex(uint32_t index)
{
    _index = index % _size;
}

template <typename T>
inline T CircularBufferReader<T>::GetElement(uint32_t index) const
{
    return _buffer[index % _size];
}

template <typename T>
inline T CircularBufferReader<T>::GetElement() const
{
    return _buffer[_index];
}

template <typename T>
inline T CircularBufferReader<T>::GetElementAndIncrementIndex()
{
    T value = _buffer[_index];
    _index = (_index + 1) % _size;
    return value;
}

#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_ */
