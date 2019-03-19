#include <hash/robinhood.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef ROBIN_HOOD_TOMBSTONE_FLAGS
#define ROBIN_HOOD_EMPTY_FLAG     0x80000000
#define ROBIN_HOOD_KEY_MASK       0x7FFFFFFF
#define ROBIN_HOOD_INVALID_DISTANCE 0xffffffff
#endif


int32_t robin_hood_hashmap_init_sized(struct robin_hood_hashmap* map_, uint32_t shift_)
{
	void* mem;
	size_t size, tmp;

	assert(map_);

	map_->shift = shift_;
	map_->count = 0;

	size = (size_t)(1U << map_->shift);
	tmp  = (sizeof(*map_->keys) + sizeof(*map_->values));
	mem  = malloc(tmp * size);

	if (mem)
	{
		map_->keys   = mem;
		map_->values = (struct robin_hood_hashmap_entry*)(map_->keys + size);

		memset(mem, 0xff, tmp * size);

		return 1;
	}
	return 0;

}
int32_t robin_hood_hashmap_init(struct robin_hood_hashmap* map_)
{
	return robin_hood_hashmap_init_sized(map_, 3);
}

void robin_hood_hashmap_deinit(struct robin_hood_hashmap* map_)
{
	assert(map_);
	free(map_->keys);
	memset(map_, 0, sizeof(*map_));
}

static int32_t robin_hood_hashmap_is_slot_empty(struct robin_hood_hashmap* map_, uint32_t idx_)
{
	return (map_->values[idx_].distance == ROBIN_HOOD_INVALID_DISTANCE);
}

static int32_t robin_hood_hashmap_increase_capacity(struct robin_hood_hashmap* map_)
{
	struct robin_hood_hashmap map;

	if (!robin_hood_hashmap_init_sized(&map, map_->shift + 1))
	{
		return 0;
	}

	for (uint32_t i = 0; i < (1U << map_->shift); ++i)
	{
		if (!robin_hood_hashmap_is_slot_empty(map_, i))
		{
			robin_hood_hashmap_insert(&map, map_->keys[i], map_->values[i].value);
		}
	}

	robin_hood_hashmap_deinit(map_);
	*map_ = map;

	return 1;
}

int32_t robin_hood_hashmap_insert(struct robin_hood_hashmap* map_, uint32_t key_, uint32_t value_)
{
	uint32_t idx;
	uint32_t dist;

	const uint32_t maxsize  = (1U << map_->shift);
	const uint32_t slotmask = maxsize - 1;

	if ((map_->count << 5) >= 30 * maxsize)
	{
		if (!robin_hood_hashmap_increase_capacity(map_))
		{
			return 0;
		}
	}

	idx  = key_ & slotmask;
	dist = 0;

	while (1)
	{
		if (map_->keys[idx] == key_)
		{
			return -1;
		}
		else 
		if (map_->values[idx].distance == ROBIN_HOOD_INVALID_DISTANCE)
		{
			map_->values[idx].distance = dist;
			map_->values[idx].value    = value_;
			map_->keys[idx]            = key_;

			break;
		}
		else if (map_->values[idx].distance < dist)
		{
			const uint32_t oldKey   = map_->keys[idx];
			const uint32_t oldValue = map_->values[idx].value;
			const uint32_t oldDist  = map_->values[idx].distance;

			map_->keys[idx]            = key_;
			map_->values[idx].distance = dist;
			map_->values[idx].value    = value_;

			key_   = oldKey;
			dist   = oldDist;
			value_ = oldValue;
		}
		else
		{
			dist++;
			idx = (idx + 1) & slotmask;
		}
	}

	map_->count++;
	return 1;
}

static void robin_hood_hashmap_clear_slot(struct robin_hood_hashmap* map_, uint32_t idx_)
{
	map_->keys[idx_]            = 0;
	map_->values[idx_].distance = ROBIN_HOOD_INVALID_DISTANCE;
	map_->values[idx_].value    = 0;
}

static void robin_hood_hashmap_swap_key_and_value(struct robin_hood_hashmap* map_, uint32_t idx0_, uint32_t idx1_)
{
	const uint32_t oldVal        = map_->values[idx0_].value;
	const uint32_t oldKey        = map_->keys[idx0_];
	const uint32_t oldDist       = map_->values[idx0_].distance;
	
	map_->values[idx0_].value    = map_->values[idx1_].value;
	map_->values[idx0_].distance = map_->values[idx1_].distance - 1;
	map_->keys[idx0_]            = map_->keys[idx1_];

	map_->values[idx1_].value    = oldVal;
	map_->values[idx1_].distance = oldDist + 1;
	map_->keys[idx1_]            = oldKey;
}

uint32_t robin_hood_hashmap_remove(struct robin_hood_hashmap* map_, uint32_t key_)
{
	const uint32_t maxsize  = (1U << map_->shift);
	const uint32_t slotmask = maxsize - 1;
	uint32_t idx            = key_ & slotmask;
	uint32_t dist           = 0;

	if(map_->count == 0)
	{ 
		return 0;
	}

	while (dist <= map_->values[idx].distance && map_->values[idx].distance != ROBIN_HOOD_INVALID_DISTANCE)
	{
		if (map_->keys[idx] == key_)
		{
			uint32_t nextIdx = (idx + 1) & slotmask;

			while(map_->values[nextIdx].distance > 0 && map_->values[nextIdx].distance < ROBIN_HOOD_INVALID_DISTANCE)
			{
				robin_hood_hashmap_swap_key_and_value(map_, idx, nextIdx);

				idx = nextIdx;
				nextIdx = (nextIdx + 1) & slotmask;
			}

			uint32_t retVal = map_->values[idx].value;
			robin_hood_hashmap_clear_slot(map_, idx);

			map_->count--;
			return retVal;
		}

		dist++;
		idx     = (idx + 1) & slotmask;
	}

	return 0;
}

uint32_t robin_hood_hashmap_find(struct robin_hood_hashmap* map_, uint32_t key_)
{
	const uint32_t maxsize  = (1U << map_->shift);
	const uint32_t slotmask = maxsize - 1;
	uint32_t idx            = key_ & slotmask;
	uint32_t dist           = 0;

	if (map_->count == 0)
	{
		return 0xffffffff;
	}

	while (1)
	{
		if (map_->keys[idx] == key_)
		{
			return map_->values[idx].value;
		}

		if (map_->values[idx].distance < dist || map_->values[idx].distance == ROBIN_HOOD_INVALID_DISTANCE)
		{
			return 0xffffffff;
		}

		dist++;
		idx = (idx + 1) & slotmask;
	}
}