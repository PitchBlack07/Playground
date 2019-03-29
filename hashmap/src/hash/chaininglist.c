#include <hash/chaininglist.h>
#include <stdlib.h>
#include <string.h>

struct chaining_list_entry
{
	struct chaining_list_entry* next;
	uint32_t                    key;
	uint32_t                    value;
};

int32_t chaining_list_hashmap_init(struct chaining_list_hashmap* hashmap_)
{
	return chaining_list_hashmap_init_sized(hashmap_, 3);
}

int32_t chaining_list_hashmap_init_sized(struct chaining_list_hashmap* hashmap_, uint32_t shift_)
{
	hashmap_->shift = shift_;
	hashmap_->size  = 0;
	hashmap_->table = malloc(sizeof(struct chaining_list_entry*) * (size_t)(1U << shift_));

	if (hashmap_->table)
	{
		memset(hashmap_->table, 0, sizeof(struct chaining_list_entry*) * (size_t)(1U << shift_));
		return 1;
	}
	return 0;
}

void chaining_list_hashmap_deinit(struct chaining_list_hashmap* hashmap_)
{
	for (uint32_t i = 0; i < (1U << hashmap_->shift); ++i)
	{
	
		struct chaining_list_entry* next = hashmap_->table[i];
		while (next)
		{
			struct chaining_list_entry* nnext = next->next;
			free(next);

			next = nnext;
		}
	}

	free(hashmap_->table);
	memset(hashmap_, 0, sizeof(*hashmap_));
}

static int32_t chaining_list_hashmap_increase_capacity(struct chaining_list_hashmap* map_)
{
	struct chaining_list_hashmap map;

	if (!chaining_list_hashmap_init_sized(&map, map_->shift + 1))
	{
		return 0;
	}

	const uint32_t capacity = (1U << map.shift);
	const uint32_t mod      = capacity - 1;

	for (uint32_t i = 0; i < (1U << map_->shift); ++i)
	{
		struct chaining_list_entry* entry = map_->table[i];
		while (entry)
		{
			struct chaining_list_entry* nextentry = entry->next;

			uint32_t idx   = entry->key & mod;
			entry->next    = map.table[idx];
			map.table[idx] = entry;

			entry = nextentry;
		}
	}

	free(map_->table);

	*map_ = map;

	return 1;
}

int32_t chaining_list_hashmap_insert(struct chaining_list_hashmap* hashmap_, uint32_t key_, uint32_t value_)
{
	const uint32_t capacity = (1U << hashmap_->shift);
	const uint32_t mod      = capacity - 1;

	if ((hashmap_->size << 5) >= 30 * capacity)
	{
		chaining_list_hashmap_increase_capacity(hashmap_);
	}

	const uint32_t idx                 = key_ & mod;
	struct chaining_list_entry** entry = &hashmap_->table[idx];

	while (*entry)
	{
		if ((*entry)->value == value_)
		{
			return -1;
		}

		entry = &(*entry)->next;
	}

	*entry = malloc(sizeof(struct chaining_list_entry));
	
	if (*entry)
	{
		(*entry)->next  = NULL;
		(*entry)->value = value_;
		(*entry)->key   = key_;

		hashmap_->size++;

		return 1;
	}

	return 0;
}

uint32_t chaining_list_hashmap_find(struct chaining_list_hashmap* hashmap_, uint32_t key_)
{
	const uint32_t capacity = (1U << hashmap_->shift);
	const uint32_t mod      = capacity - 1;
	const uint32_t idx      = key_ & mod;

	struct chaining_list_entry* entry = hashmap_->table[idx];
	while (entry)
	{
		if (entry->key == key_)
		{
			return entry->value;
		}
		entry = entry->next;
	}
	return 0;
}

uint32_t chaining_list_hashmap_remove(struct chaining_list_hashmap* hashmap_, uint32_t key_)
{
	const uint32_t capacity = (1U << hashmap_->shift);
	const uint32_t mod      = capacity - 1;
	const uint32_t idx      = key_ & mod;

	struct chaining_list_entry* prev  = NULL;
	struct chaining_list_entry* entry = hashmap_->table[idx];

	while (entry)
	{
		if (entry->key == key_)
		{
			if (prev)
			{
				prev->next = entry->next;
			}
			else
			{
				hashmap_->table[idx] = entry->next;
			}
			uint32_t retVal = entry->value;
			free(entry);
			return retVal;
		}
		prev  = entry;
		entry = entry->next;
	}
	return 0xffffffff;
}

uint32_t chaining_list_hashmap_keycount(struct chaining_list_hashmap* hashmap_, uint32_t slot_)
{
	const uint32_t capacity = (1U << hashmap_->shift);
	const uint32_t mod      = capacity - 1;
	const uint32_t idx      = slot_ & mod;

	struct chaining_list_entry* entry = hashmap_->table[idx];
	uint32_t cnt = 0;
	while (entry)
	{
		if (entry->key == slot_)
		{
			return cnt;
		}
		cnt++;
		entry = entry->next;
	}
	return cnt;
}