#pragma once

#include <stdint.h>

struct chaining_list_entry;

struct chaining_list_hashmap
{
	struct chaining_list_entry** table;
	uint32_t                     size;
	uint32_t                     shift;
};

int32_t chaining_list_hashmap_init(struct chaining_list_hashmap* hashmap_);
int32_t chaining_list_hashmap_init_sized(struct chaining_list_hashmap* hashmap_, uint32_t shift_);

uint32_t chaining_list_hashmap_keycount(struct chaining_list_hashmap* hashmap_, uint32_t slot_);

int32_t chaining_list_hashmap_insert(struct chaining_list_hashmap* hashmap_, uint32_t key_, uint32_t value_);
uint32_t chaining_list_hashmap_find(struct chaining_list_hashmap* hashmap_, uint32_t key_);
uint32_t chaining_list_hashmap_remove(struct chaining_list_hashmap* hashmap_, uint32_t key_);

void chaining_list_hashmap_deinit(struct chaining_list_hashmap* hashmap_);