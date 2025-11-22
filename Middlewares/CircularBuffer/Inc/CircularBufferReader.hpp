#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_

#include <stdint.h>

#include "MessagePassing/circular_buffers.h"

template <typename T>
class CircularBufferReader
{
public:
    CircularBufferReader(T* buffer, circular_buffer_config* cfg);

    // Index management
    uint32_t GetIndex() const;
    void SetIndex(uint32_t index);

    // Element access
    T GetElement(uint32_t index) const;
    T GetElement(T& out) const;
    T GetElementAndIncrementIndex(T& out);

private:
    T* _buffer;           // external buffer memory that we're reading from
    circular_buffer_config* _cfg; // what data type we associate with current read
    
};

template <typename T>
inline CircularBufferReader<T>::CircularBufferReader(T* buffer, circular_buffer_config* cfg)
    : _buffer(buffer), _cfg(cfg)
{
}

template <typename T>
inline uint32_t CircularBufferReader<T>::GetIndex() const
{
    return _cfg->readerIndex;
}

template <typename T>
inline void CircularBufferReader<T>::SetIndex(uint32_t index)
{
    _cfg->readerIndex = index % _cfg->size;
}

template <typename T>
inline T CircularBufferReader<T>::GetElement(uint32_t index) const
{
    return _buffer[index % _cfg->size];
}

template <typename T>
inline bool CircularBufferReader<T>::GetElement(T& out) const
{
    // empty: nothing new to read
    if (_cfg->readerIndex == _cfg->writerIndex)
        return false;

    out = _buffer[_cfg->readerIndex];
    return true;
}

template <typename T>
inline bool CircularBufferReader<T>::GetElementAndIncrementIndex(T& out)
{
    // empty: nothing new to read
    if (_cfg->readerIndex == _cfg->writerIndex)
        return false;

    out = _buffer[_cfg->readerIndex];

    // advance read index
    _cfg->readerIndex = (_cfg->readerIndex + 1) % _cfg->size;

    return true;
}


#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_ */
