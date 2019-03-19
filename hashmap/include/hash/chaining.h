#pragma once

#include <stdint.h>


struct chaining_hashmap_entry;

struct chaining_hashmap
{
	struct chaining_hashmap_entry* table;
	uint32_t                       shift;
	uint32_t                       size;
};

int32_t chaining_hashmap_init(struct chaining_hashmap* hashmap_);
int32_t chaining_hashmap_init_sized(struct chaining_hashmap* hashmap_, uint32_t shift_);

uint32_t chaining_hashmap_keycount(struct chaining_hashmap* hashmap_, uint32_t slot_);

int32_t chaining_hashmap_insert(struct chaining_hashmap* hashmap_, uint32_t key_, uint32_t value_);
uint32_t chaining_hashmap_find(struct chaining_hashmap* map_, uint32_t key_);
uint32_t chaining_hashmap_remove(struct chaining_hashmap* map_, uint32_t key_);
void chaining_hashmap_deinit(struct chaining_hashmap* hashmap_);