#include <sys/types.h>
#include <string.h>
#include "hash_t.h"
#define make_hash_t(TYPE) make_hash_t_t(sizeof(TYPE))
#define get_hash_t(TABLE, KEY, TYPE) ((TYPE*)get_hash_t_t(TABLE, KEY))

struct hash_t make_hash_t_t(size_t sz)
{
	struct hash_t ht;
	ht.val = make_vector_t(sz);
	ht.key = make_vector(string);
	ht.size = 0;
	reserve_vector(&ht.val, 8);
	reserve_vector(&ht.key, 8);
	ht.capacity = 8;
	return ht;
}

unsigned long elfHash(const unsigned char *s)
{	
	const unsigned long BYTES_UL = sizeof(unsigned long);
	unsigned long h = 0, high;
	while (*s)
	{
		h = (h << BYTES_UL) + *s++;
		if ((high = h & ~ 0UL << BYTES_UL * 7))
			h ^= high >> BYTES_UL * 6;
		h &= ~high;
	}
	return h;
}

void hash_add(struct hash_t *t, string key, void *val)
{
	size_t i = elfHash((const unsigned char*) key.val) % t->capacity;
	string *key_p = begin_vector(&t->key, string);
	while (((key_p =  begin_vector(&t->key, string) + i)->val) && strcmp(key_p->val, key.val) != 0)
		if (++i == t->capacity)
			i = 0;
	begin_vector(&t->key, string)[i] = key;
	char *pval = (char*) val;
	for (size_t j = 0; j < t->val.sz; ++j)
		begin_vector(&t->val, char)[j + i * t->val.sz] = pval[j];
	++t->size;
}

void resize_hash_t(struct hash_t *t, size_t n)
{
	struct hash_t new_t = make_hash_t_t(t->val.sz);
	reserve_vector(&new_t.val, n);
	reserve_vector(&new_t.key, n);
	new_t.capacity = n;
	new_t.size = t->size;
	string *p = 0;
	for (size_t i = 0; i < t->key.capacity; ++i)
	{
		p = begin_vector(&t->key, string) + i;
		if (p->val)
			hash_add(&new_t, *p, begin_vector(&t->val, char) + i * t->val.sz);
	}
	free_vector(&t->val);
	free_vector(&t->key);
	*t = new_t;
}
void add_hash_t(struct hash_t *t, string key, void *val)
{
	if (strlen(key.val) == 0)
		return;
	if (t->size >= t->capacity / 4)
	{
		t->capacity *= 2;
		resize_hash_t(t, t->capacity);
	}
	hash_add(t, key, val);
}

void* get_hash_t_t(struct hash_t *t, string key)
{
	if (!key.val)
		return 0;
	size_t i = elfHash((const unsigned char*)key.val) % t->capacity;
	string *pstr = 0;
	while ((pstr = begin_vector(&t->key, string) + i)->val && strcmp(begin_vector(&t->key, string)[i].val, key.val) != 0)
		if (++i == t->key.capacity)
			i = 0;
	if (!pstr->val)
		return 0;
	return begin_vector(&t->val, char) + i * t->val.sz;
}

void free_hash_t(struct hash_t *t)
{
	for (size_t i = 0; i < t->capacity; ++i)
	{
		string *p = 0;
		if ((p = begin_vector(&t->key, string) + i)->size != 0)
			free_string(p);
	}
	free_vector(&t->key);
	free_vector(&t->val);
	t->size = 0;
	t->capacity = 0;
}


/*int main()
{
	struct hash_t table = make_hash_t(int);
	char key[1000];
	int val;
	for (size_t i = 0; i < 5; ++i)
	{
		scanf("%s%d", key, &val);
		add_hash_t(&table, make_string(key), &val);
	}
	while (scanf("%s", key) == 1)
	{
		int *p = get_hash_t(&table, make_string(key), int);
		if (p)
			printf("%d\n", *get_hash_t(&table, make_string(key), int));
		else
			printf("not found");
	}
	free_hash_t(&table);
	return 0;
}*/
