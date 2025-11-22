#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_

#include <stdint.h>

template <typename T>
class CircularBufferWriter
{
public:
    CircularBufferWriter(T* buffer, circular_buffer_config* cfg);

    // Index management
    uint32_t GetIndex() const;
    void SetIndex(uint32_t index);

    // Element write
    void WriteElement(const T& value);
    void WriteElement(uint32_t index, const T& value);
    void WriteElementAndIncrementIndex(const T& value);

private:
    T* _buffer;        // external buffer memory
    circular_buffer_config* _cfg;
};

template <typename T>
inline CircularBufferWriter<T>::CircularBufferWriter(T* buffer, circular_buffer_config* cfg)
    : _buffer(buffer), _cfg(cfg)
{
}

template <typename T>
inline uint32_t CircularBufferWriter<T>::GetIndex() const
{
    return _cfg->writerIndex;
}

template <typename T>
inline void CircularBufferWriter<T>::SetIndex(uint32_t index)
{
    _cfg->writerIndex = index % _cfg->size;
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElement(const T& value)
{
    _buffer[_cfg->writerIndex] = value;
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElement(uint32_t index, const T& value)
{
    _buffer[index % _cfg->size] = value;
}

template <typename T>
inline void CircularBufferWriter<T>::WriteElementAndIncrementIndex(const T& value)
{
    _buffer[_cfg->writerIndex] = value;
    _cfg->writerIndex = (_cfg->writerIndex + 1) % _cfg->size;
}

#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERWRITER_HPP_ */
