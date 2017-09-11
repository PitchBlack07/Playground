#pragma once

#include <stdint.h>
#include <stdlib.h>

template<unsigned int a> 
uint32_t align(uint32_t s_)
{
	return (s_ + a - 1) & ~(a - 1);
}

template<unsigned int size, unsigned int alignment>
class ring_buffer
{
public:
	ring_buffer();

	void* alloc(uint32_t size_);
	void clear(uint32_t size_);

public:
	char     buffer[size];
	uint32_t head;
	uint32_t tail;
	uint32_t free;
};

template<unsigned int size, unsigned int alignment>
ring_buffer<size, alignment>::ring_buffer()
	: head(0)
	, tail(0)
	, free(size)
{
}

template<unsigned int size, unsigned int alignment>
auto ring_buffer<size, alignment>::alloc(uint32_t size_) -> void*
{
	const uint32_t alignedsize = align<alignment>(size_);
	void* retval               = nullptr;

	if (free >= size_) {
		if (head >= tail) {
			if (head + alignedsize <= _countof(buffer)) {
				retval = buffer + head;
				head  += alignedsize;
				free  -= alignedsize;
			}
			else if (tail >= alignedsize) {
				retval = buffer;
				free -= alignedsize + (_countof(buffer) - head);
				head = alignedsize;
			}
		}
		else //(head + alignedsize <= tail) 
		{
			retval = buffer + head;
			free -= alignedsize;
			head += alignedsize;
		}
	}

	return retval;
}

template<unsigned int size, unsigned int alignment>
void ring_buffer<size, alignment>::clear(uint32_t size_)
{
	const uint32_t clearsize = align<alignment>(size_);
	if (clearsize <= _countof(buffer) - free)
	{
		tail += clearsize;
		free += clearsize;
		tail %= _countof(buffer);
	}
}