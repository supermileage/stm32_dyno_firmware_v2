#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

template <typename T>
class CircularBufferWriter
{
public:
    CircularBufferWriter(T* buffer, uint32_t* writerIndex, uint32_t size);

    // Index management
    uint32_t GetIndex() const;
    void SetIndex(uint32_t index);

    // Element write
    void WriteElement(const T& value);
    void WriteElement(uint32_t index, const T& value);
    void WriteElementAndIncrementIndex(const T& value);

private:
    T* _buffer;        // external buffer memory
    uint32_t* _writerIndex;
    uint32_t _size;
};

template <typename T>
inline CircularBufferWriter<T>::CircularBufferWriter(T* buffer, uint32_t* writerIndex, uint32_t size)
    : _buffer(buffer), _writerIndex(writerIndex), _size(size)
{
	*_writerIndex = 0;
}

template <typename T>
inline uint32_t CircularBufferWriter<T>::GetIndex() const
{
    taskENTER_CRITICAL(); 
    uint32_t writerIndex = *_writerIndex;
    taskEXIT_CRITICAL(); 
    return writerIndex;
}

template <typename T>
inline void CircularBufferWriter<T>::SetIndex(uint32_t index)
{
    taskENTER_CRITICAL(); 
    *_writerIndex = index % _size;
    taskEXIT_CRITICAL(); 
}


template <typename T>
inline void CircularBufferWriter<T>::WriteElement(const T& value)
{
    taskENTER_CRITICAL(); 
    _buffer[*_writerIndex] = value;
    taskEXIT_CRITICAL();
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElement(uint32_t index, const T& value)
{
    taskENTER_CRITICAL(); 
    _buffer[index % _size] = value;
    taskEXIT_CRITICAL(); 
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElementAndIncrementIndex(const T& value)
{
    taskENTER_CRITICAL(); 
    _buffer[*_writerIndex] = value;
    *_writerIndex = (*_writerIndex + 1) % _size;
    taskEXIT_CRITICAL(); 
}

#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_ */
