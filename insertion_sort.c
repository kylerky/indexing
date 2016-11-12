#include "sort.h"
#include "vector.h"

void insertion_sort(void *left, void *right, int (*cmp)(void*, void*), size_t sz)
{
	if (left >= right)
		return;
	for (char *i = (char*)left + sz; i != right; i += sz)
	{
		char key[sz];
		for (size_t j = 0; j < sz; ++j)
			key[j] = i[j];
		char *k = i;
		while (k != left && (*cmp)(key, k - sz) < 0)
		{
			for (size_t j = 0; j < sz; ++j)
				k[j] = k[j - sz];
			k -= sz;
		}
		for (size_t j = 0; j < sz; ++j)
			k[j] = key[j];
	}
}
