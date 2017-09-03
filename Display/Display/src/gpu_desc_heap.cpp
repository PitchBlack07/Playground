#include "gpu.h"
#include "gpu_desc_heap.h"
#include "utils.h"
#include <nmmintrin.h>

static UINT get_desc_count(UINT64 mask_)
{
	const UINT* const u32 = (UINT*)&mask_;

	return _mm_popcnt_u32(u32[0]) + _mm_popcnt_u32(u32[1]);
}
