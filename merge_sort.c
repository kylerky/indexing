#include "vector.h"
#include "sort.h"

void merge(char*, char*, char*, int (*)(void*, void*), size_t);
void elem_copy(char*, char*, size_t sz);
void merge_sort(void *left, void *right, int (*cmp)(void*, void*), size_t sz)
{
	if (left + 10 * sz >= right)
		return insertion_sort(left, right, cmp, sz);
	merge_sort(left, (void*)((char*)left + ((char*)right - (char*)left) / sz / 2 * sz), cmp, sz);
	merge_sort((void*)((char*)left + ((char*)right - (char*)left) / sz / 2 * sz), right, cmp, sz);
	merge(left, ((char*)left + ((char*)right - (char*)left) / sz / 2 * sz), right, cmp, sz);
}

void merge(char *left, char *mid, char *right, int (*cmp)(void*, void*), size_t sz)
{
	struct vector aux = make_vector_t(sz);
	for (char * i = left; i != right; i += sz)
		push_vector(&aux, (void*) i);
	char *j = begin_vector(&aux, char), *k = j + (mid - left), *auxmid = j + (mid - left), *auxright = end_vector(&aux, char);
	for (char *i = left; i != right; i+=sz)
	{
		if (j == auxmid)
		{
			elem_copy(i, k, sz);
			k += sz;
		}
		else if (k == auxright)
		{
			elem_copy(i, j, sz);
			j += sz;
		}
		else if ((*cmp)(j, k) <= 0)
		{
			elem_copy(i, j, sz);
			j += sz;
		}
		else
		{
			elem_copy(i, k, sz);
			k += sz;
		}

	}
	free_vector(&aux);
}

void elem_copy(char *dest, char *src, size_t sz)
{
	for (size_t i = 0; i < sz; ++i)
		dest[i] = src[i];
}
