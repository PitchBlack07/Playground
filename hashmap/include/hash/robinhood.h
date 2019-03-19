#ifndef ROBINHOOD_HASHMAP_H
#define ROBINHOOD_HASHMAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct robin_hood_hashmap_entry
	{
		uint32_t distance;
		uint32_t value;
	};

	struct robin_hood_hashmap
	{
		uint32_t*                        keys;
		struct robin_hood_hashmap_entry* values;

		uint32_t shift;
		uint32_t count;
	};
	
	int32_t robin_hood_hashmap_init(struct robin_hood_hashmap* map_);
	int32_t robin_hood_hashmap_init_sized(struct robin_hood_hashmap* map_, uint32_t shift_);
	void robin_hood_hashmap_deinit(struct robin_hood_hashmap* map_);

	int32_t robin_hood_hashmap_insert(struct robin_hood_hashmap* map_, uint32_t key_, uint32_t value_);
	uint32_t robin_hood_hashmap_find(struct robin_hood_hashmap* map_, uint32_t key_);
	uint32_t robin_hood_hashmap_remove(struct robin_hood_hashmap* map_, uint32_t key_);
	

#ifdef __cplusplus
}
#endif

#endif