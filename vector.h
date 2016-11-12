#include <sys/types.h>

#ifndef VECTOR_H
#define VECTOR_H

#define make_vector(TYPE) make_vector_t(sizeof(TYPE))
#define get_p_vector(VEC, INDEX, TYPE) ((TYPE*) ((VEC)->val + INDEX * (VEC)->sz))
#define begin_vector(VEC, TYPE) get_p_vector(VEC, 0, TYPE)
#define end_vector(VEC, TYPE) get_p_vector(VEC, (VEC)->size, TYPE)
struct vector
{
	char *val;
	size_t size;
	size_t capacity;
	size_t sz;
};

struct vector make_vector_t(size_t);

void reserve_vector(struct vector *vec, size_t n);

void push_vector(struct vector *vec, void *val);

void pop_vector(struct vector *vec);

void free_vector(struct vector *vec);
#endif
