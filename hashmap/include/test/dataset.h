#pragma once

#include <stdint.h>

struct dataset
{
	uint32_t key;
	uint32_t value;
};

struct dataset* generate_random_data_set(uint32_t count_);

void shuffle_data_set(struct dataset* dataset_, uint32_t count_);

void shuffle_uint32(uint32_t* data_, uint32_t count_);