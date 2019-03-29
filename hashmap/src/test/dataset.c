
#include <stdlib.h>
#include <hash/fnv1a.h>
#include <pcg/pcg_basic.h>
#include <test/dataset.h>

struct dataset* generate_random_data_set(uint32_t count_)
{
	uint32_t i;
	struct dataset* ds = malloc(sizeof(struct dataset) * count_);
	if (!ds) {
		return NULL;
	}

	for (i = 0; i < count_; ++i) {
		uint32_t rnd = pcg32_random();

		ds[i].key = fnv1a32(&rnd, sizeof(rnd), FNV_OFFSET_BIAS);
		ds[i].value = rnd & 0xffffff;
	}

	return ds;
}

void shuffle_data_set(struct dataset* dataset_, uint32_t count_)
{
	uint32_t i;
	for (i = count_; i > 1; i--)
	{
		const uint32_t swapidx = i - 1;
		const uint32_t rndidx = pcg32_boundedrand(swapidx);

		struct dataset oldSet = dataset_[swapidx];
		dataset_[swapidx] = dataset_[rndidx];
		dataset_[rndidx] = oldSet;
	}
}

void shuffle_uint32(uint32_t* data_, uint32_t count_)
{
	uint32_t i;
	for (i = count_; i > 1; i--)
	{
		const uint32_t swapidx = i - 1;
		const uint32_t rndidx  = pcg32_boundedrand(swapidx);

		uint32_t oldint = data_[swapidx];
		data_[swapidx]  = data_[rndidx];
		data_[rndidx]   = oldint;
	}
}