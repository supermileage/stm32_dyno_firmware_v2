#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_

#include <stdint.h>

#include "MessagePassing/circular_buffers.h"

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
    uint32_t _readerIndex;
    uint32_t _size;
};

template <typename T>
inline CircularBufferReader<T>::CircularBufferReader(T* buffer, uint32_t* writerIndex, uint32_t size)
    : _buffer(buffer), _writerIndex(writerIndex), _size(size), _readerIndex(0)
{
}

template <typename T>
inline uint32_t CircularBufferReader<T>::GetIndex() const
{
    return _readerIndex;
}

template <typename T>
inline void CircularBufferReader<T>::SetIndex(uint32_t index)
{
	_readerIndex = index % _size;
}

template <typename T>
inline T CircularBufferReader<T>::GetElement(uint32_t index) const
{
    return _buffer[index % _size];
}

template <typename T>
inline bool CircularBufferReader<T>::GetElement(T& out) const
{
    // empty: nothing new to read
    if (_readerIndex == *_writerIndex)
        return false;

    out = _buffer[_readerIndex];
    return true;
}

template <typename T>
inline bool CircularBufferReader<T>::GetElementAndIncrementIndex(T& out)
{
    // empty: nothing new to read
    if (_readerIndex == *_writerIndex)
        return false;

    out = _buffer[_readerIndex];

    // advance read index
    _readerIndex = (_readerIndex + 1) % _size;

    return true;
}


#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_ */
