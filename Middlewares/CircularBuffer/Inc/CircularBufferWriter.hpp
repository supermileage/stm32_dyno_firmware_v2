#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_

#include <stdint.h>

/*
 * CircularBufferWriter
 * ---------------------
 * Writes into an externally allocated buffer.
 * Does NOT own the memory.
 * Fully embedded-safe (no STL).
 */

template <typename T>
class CircularBufferWriter
{
public:
    CircularBufferWriter(T* buffer, uint32_t size);

    // Index management
    uint32_t GetIndex() const;
    void SetIndex(uint32_t index);

    // Element write
    void WriteElement(const T& value);
    void WriteElement(uint32_t index, const T& value);
    void WriteElementAndIncrementIndex(const T& value);

private:
    T* _buffer;        // external buffer memory
    uint32_t _size;    // number of elements
    uint32_t _index;   // current write index
};

// =======================
// Template Implementation
// =======================

template <typename T>
inline CircularBufferWriter<T>::CircularBufferWriter(T* buffer, uint32_t size)
    : _buffer(buffer), _size(size), _index(0)
{
}

template <typename T>
inline uint32_t CircularBufferWriter<T>::GetIndex() const
{
    return _index;
}

template <typename T>
inline void CircularBufferWriter<T>::SetIndex(uint32_t index)
{
    _index = index % _size;
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElement(const T& value)
{
    _buffer[_index] = value;
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElement(uint32_t index, const T& value)
{
    _buffer[index % _size] = value;
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElementAndIncrementIndex(const T& value)
{
    _buffer[_index] = value;
    _index = (_index + 1) % _size;
}

#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_ */
