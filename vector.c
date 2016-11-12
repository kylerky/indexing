#include "vector.h"
#include <stdlib.h>

static const int void_size = sizeof(void*);

struct vector make_vector_t(size_t sz)
{
	struct vector vec;
	if ((vec.val = (char*) calloc(2, sz)))
	{
		vec.size = 0;
		vec.capacity = 2;
		vec.sz = sz;
	}
	else
	{
		vec.size = 0;
		vec.capacity = 0;
	}
	return vec;
}

static void resize(struct vector *vec, size_t n)
{
	size_t size = vec->size;
	char *new_vec = (char *) calloc(n, vec->sz);
	for (size_t i = 0; i < size; ++i)
		for (size_t j = 0; j < vec->sz; ++j)
			new_vec[i * vec->sz + j] = vec->val[i * vec->sz + j];
	free_vector(vec);
	vec->val = new_vec;
	vec->size = size;
	vec->capacity = n;
}

void reserve_vector(struct vector *vec, size_t n)
{
	if (vec->capacity <= 0)
		return;
	resize(vec, n);
	vec->size = vec->capacity;
}

void push_vector(struct vector *vec, void *val)
{
	if (vec->capacity <= 0)
		return;
	if (vec->size == vec->capacity)
		resize(vec, vec->capacity * 2);
	char *pval = (char*) val;
	for (size_t i = 0; i < vec->sz; ++i)
		vec->val[i + vec->size * vec->sz] = pval[i];
	++vec->size;
}

void pop_vector(struct vector *vec)
{
	if (vec->size < 1)
		return;
	--vec->size;
	if (vec->size < vec->capacity / 4)
		resize(vec, vec->capacity / 2);
}

void free_vector(struct vector *vec)
{
	free((void*) vec->val);
	vec->capacity = 0;
	vec->size = 0;
}

