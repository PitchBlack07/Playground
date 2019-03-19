#include <hash/chaining.h>
#include <stdlib.h>
#include <string.h>

struct key_value_pair
{
	uint32_t key;
	uint32_t value;
};

struct chaining_hashmap_entry
{
	union
	{
		struct key_value_pair* items;
		struct key_value_pair  item;
	};
	uint16_t size;
	uint16_t shift;
};

int32_t chaining_hashmap_init(struct chaining_hashmap* hashmap_)
{
	return chaining_hashmap_init_sized(hashmap_, 3);
}

int32_t chaining_hashmap_init_sized(struct chaining_hashmap* hashmap_, uint32_t size_)
{
	hashmap_->shift = size_;
	hashmap_->size  = 0;
	hashmap_->table = malloc(sizeof(struct chaining_hashmap_entry) * (size_t)(1U << size_));

	if (hashmap_->table)
	{
		memset(hashmap_->table, 0, sizeof(struct chaining_hashmap_entry) * (size_t)(1U << size_));
		return 1;
	}
	return 0;
}

void chaining_hashmap_deinit(struct chaining_hashmap* hashmap_)
{
	for (uint32_t i = 0; i < (1U << hashmap_->shift); ++i)
	{
		if (hashmap_->table[i].size > 1)
		{
			free(hashmap_->table[i].items);
		}
	}
	free(hashmap_->table);
	memset(hashmap_, 0, sizeof(*hashmap_));
}

static int32_t chaining_hashmap_increase_capacity(struct chaining_hashmap* map_)
{
	struct chaining_hashmap map;

	if (!chaining_hashmap_init_sized(&map, map_->shift + 1))
	{
		return 0;
	}

	for (uint32_t i = 0; i < (1U << map_->shift); ++i)
	{
		if (map_->table[i].size == 1)
		{
			chaining_hashmap_insert(&map, map_->table[i].item.key, map_->table[i].item.value);
		}
		else if(map_->table[i].size > 1)
		{
			for (uint32_t j = 0; j < map_->table[i].size; ++j)
			{
				chaining_hashmap_insert(&map, map_->table[i].items[j].key, map_->table[i].items[j].value);
			}
			free(map_->table[i].items);
		}
	}

	free(map_->table);

	*map_ = map;

	return 1;
}



int32_t chaining_hashmap_insert(struct chaining_hashmap* hashmap_, uint32_t key_, uint32_t value_)
{
	const uint32_t capacity = (1U << hashmap_->shift);
	const uint32_t mod      = capacity - 1;

	if ((hashmap_->size << 5) >= 30 * capacity)
	{
		chaining_hashmap_increase_capacity(hashmap_);
	}

	const uint32_t idx = key_ & mod;
	struct chaining_hashmap_entry* const entry = &hashmap_->table[idx];
	
	if (entry->size == 0)
	{
		entry->item.key   = key_;
		entry->item.value = value_;
		entry->size       = 1;
		entry->shift      = 0;

		hashmap_->size++;

		return 1;
	}
	else if(entry->size == 1)
	{
		if (entry->item.key == key_)
		{
			return -1;
		}


		struct key_value_pair* list = malloc(sizeof(struct key_value_pair) * 4);
		if(list == NULL)
		{ 
			return 0;
		}

		list[0].key   = entry->item.key;
		list[0].value = entry->item.value;
		list[1].key   = key_;
		list[1].value = value_;

		entry->size  = 2;
		entry->shift = 2;
		entry->items = list;
		hashmap_->size++;
		return 1;
	}
	else
	{
		for (uint32_t i = 0; i < entry->size; ++i)
		{
			if (entry->items[i].key == key_)
			{
				return -1;
			}
		}

		if (entry->size == (1U << entry->shift))
		{
			entry->shift++;
			entry->items = realloc(entry->items, sizeof(struct key_value_pair) * (size_t)(1U << entry->shift));
			if (entry->items == NULL)
			{
				return 0;
			}
		}

		const uint32_t s      = entry->size;
		entry->items[s].key   = key_;
		entry->items[s].value = value_;
		entry->size           = s + 1;

		hashmap_->size++;

		return 1;
	}
}

uint32_t chaining_hashmap_find(struct chaining_hashmap* map_, uint32_t key_)
{
	const uint32_t capacity = (1U << map_->shift);
	const uint32_t mod      = capacity - 1;
	const uint32_t idx      = key_ & mod;

	const struct chaining_hashmap_entry* const entry = map_->table + idx;

	if (entry->size == 0)
	{
		return 0xffffffff;
	}

	if (entry->size == 1)
	{
		if (entry->item.key == key_)
		{
			return entry->item.value;
		}
		else
		{
			return 0xffffffff;
		}
	}

	for (uint32_t i = 0; i < entry->size; ++i)
	{
		if (entry->items[i].key == key_)
		{
			return entry->items[i].value;
		}
	}
	return 0xffffffff;
}

uint32_t chaining_hashmap_remove(struct chaining_hashmap* map_, uint32_t key_)
{
	const uint32_t capacity = (1U << map_->shift);
	const uint32_t mod      = capacity - 1;
	const uint32_t idx      = key_ & mod;

	struct chaining_hashmap_entry* const entry = map_->table + idx;

	if (entry->size == 0)
	{
		return 0;
	}

	if (entry->size == 1)
	{
		entry->size = 0;
		map_->size--;
		return entry->item.value;
	}

	uint32_t i, retVal = 0;
	for (i = 0; i < entry->size; ++i)
	{
		if (entry->items[i].key == key_)
		{
			retVal = entry->items[i].value;
			entry->size--;

			if (entry->size == 1)
			{
				struct key_value_pair tmp = entry->items[(i + 1) % (1U << entry->shift)];
			
				free(entry->items);
				entry->item = tmp;
			}
			else
			{
				entry->items[i] = entry->items[entry->size];

				if (entry->size == (1U << (entry->shift - 1)))
				{
					entry->items = realloc(entry->items, sizeof(struct key_value_pair) * (size_t)(1U << (entry->shift - 1)));
					entry->shift--;
				}
			}

			map_->size--;
			return retVal;
		}
	}

	return 0;
}

uint32_t chaining_hashmap_keycount(struct chaining_hashmap* hashmap_, uint32_t slot_)
{
	return hashmap_->table[slot_].size;
}