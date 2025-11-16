#ifndef CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_
#define CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_

#include <stdint.h>

template <typename T>
class CircularBufferReader
{
	public:
		CircularBufferReader(uint32_t array_size);
		~CircularBufferReader() = default;

		std::iterator GetIterator();
		T GetElement(std::iterator it);

	private:
		T buffer;

		std::iterator<T> _iterator;
		const uint32_t _array_size;
};

#endif /* CIRCULARBUFFER_INC_CIRCULARBUFFERREADER_HPP_ */
