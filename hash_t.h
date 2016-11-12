#ifndef HASH_T_H
#define HASH_T_H

#include "vector.h"
#include "str.h"

#define make_hash_t(TYPE) make_hash_t_t(sizeof(TYPE))
#define get_hash_t(TABLE, KEY, TYPE) ((TYPE*)get_hash_t_t(TABLE, KEY))

struct hash_t
{
	struct vector key;
	struct vector val;
	size_t size;
	size_t capacity;
};
struct hash_t make_hash_t_t(size_t sz);
void add_hash_t(struct hash_t *t, string key, void *val);
void* get_hash_t_t(struct hash_t *t, string key);
void free_hash_t(struct hash_t *t);
#endif
