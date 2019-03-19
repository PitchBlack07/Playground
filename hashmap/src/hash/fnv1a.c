
#include <hash/fnv1a.h>

#define FNV_32_PRIME ((uint32_t)0x01000193)

uint32_t fnv1a32(const void* data_, uint32_t dataSizeInBytes_, uint32_t offset_)
{
	const unsigned char *bp = (unsigned char *)data_;
	const unsigned char *be = bp + dataSizeInBytes_;		

	while (bp < be) {
		offset_ ^= (uint32_t)*bp++;
		offset_ *= FNV_32_PRIME;
	}

	return offset_;
}