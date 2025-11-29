#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

template <typename T>
class CircularBufferReader
{
public:
    CircularBufferReader(T* buffer, uint32_t* writerIndex, uint32_t size);

    // Index management
    uint32_t GetIndex() const;
	void SetIndex(uint32_t index);

	T GetElement(uint32_t index) const;
	bool GetElement(T& out) const;
	bool GetElementAndIncrementIndex(T& out);

private:
    T* _buffer;           // external buffer memory
    uint32_t* _writerIndex;
    uint32_t _size; 
    uint32_t _readerIndex;

};

template <typename T>
inline CircularBufferReader<T>::CircularBufferReader(T* buffer, uint32_t* writerIndex, uint32_t size)
    : _buffer(buffer), _writerIndex(writerIndex), _size(size), _readerIndex(0)
{
}

template <typename T>
inline uint32_t CircularBufferReader<T>::GetIndex() const
{
    taskENTER_CRITICAL(); 
    uint32_t readerIndex = _readerIndex;
    taskEXIT_CRITICAL(); 
    return readerIndex;
}

template <typename T>
inline void CircularBufferReader<T>::SetIndex(uint32_t index)
{
	taskENTER_CRITICAL(); 
    _readerIndex = index % _size;
    taskEXIT_CRITICAL();
}

template <typename T>
inline T CircularBufferReader<T>::GetElement(uint32_t index) const
{
    taskENTER_CRITICAL(); 
    return _buffer[index % _size];
    taskEXIT_CRITICAL();
}

template <typename T>
inline bool CircularBufferReader<T>::GetElement(T& out) const
{
    taskENTER_CRITICAL(); 
    
    // empty: nothing new to read
    if (_readerIndex == *_writerIndex)
    {
        taskEXIT_CRITICAL();
        return false;
    }

    out = _buffer[_readerIndex];
    taskEXIT_CRITICAL();
    return true;
}

template <typename T>
inline bool CircularBufferReader<T>::GetElementAndIncrementIndex(T& out)
{
    taskENTER_CRITICAL();                // disable context switch
    if (_readerIndex == *_writerIndex)
    {
        taskEXIT_CRITICAL();
        return false;
    }
    out = _buffer[_readerIndex];         // read full struct
    _readerIndex = (_readerIndex + 1) % _size;
    taskEXIT_CRITICAL();
    return true;
}



#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_ */
