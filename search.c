#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "str.h"
#include "sort.h"
#include "path.h"
#include "hash_t.h"

int voidpstrcmp(void *a, void *b)
{
	return strcmp(*(const char**)a, *(const char**)b);
}

void* binary_search(void *left, void *right, void *val, int (*cmp)(void*, void*), size_t sz)
{
	if (left >= right)
		return 0;
	void *mid = (char*)left + ((char*)right - (char*)left) / sz / 2 * sz;
	int result = (*cmp)(val, (void*) mid);
	if (result == 0)
		return mid;
	else if (result < 0)
		return binary_search(left, mid, val, cmp, sz);
	else
		return binary_search(mid + sz, right, val, cmp, sz);
}

struct index_node
{
	string index;
	size_t count;
};

struct result_node
{
	string file_name;
	struct vector indexes;
	size_t count;
};

int index_cnt_cmp(void *a, void *b)
{
	struct result_node *na = a, *nb = b;
	if (na->indexes.size == nb->indexes.size)
		return 0;
	else if (na->indexes.size < nb->indexes.size)
		return 1;
	else
		return -1;
}
int file_cnt_cmp(void *a, void *b)
{
	struct result_node *na = a, *nb = b;
	if (na->count == nb->count)
		return 0;
	else if (na->count < nb->count)
		return 1;
	else
		return -1;
}
void get_result(string index, size_t root, struct hash_t *storage, FILE *fp, fpos_t fn_pos)
{
	fseek(fp, 1L + sizeof(size_t) + root * 4 * sizeof(size_t), SEEK_SET);
	size_t in[4];
	fpos_t index_cur;
	while (fread(in, sizeof(size_t), 4, fp))
	{
		fgetpos(fp, &index_cur);
		fsetpos(fp, &fn_pos);
		fseek(fp, in[0], SEEK_CUR);
		string fn = make_vector(char);
		char letter = 0;
		for (size_t i = 0; i < in[1]; ++i)
		{
			letter = (char)fgetc(fp);
			push_vector(&fn, &letter);
		}
		push_vector(&fn, "");
		if (get_hash_t(storage, fn, struct result_node) == 0)
		{
			struct result_node tmp;
			tmp.file_name = fn;
			tmp.count = 0;
			tmp.indexes = make_vector(struct index_node);
			add_hash_t(storage, fn, &tmp);
		}
		struct result_node *rn = get_hash_t(storage, fn, struct result_node);
		rn->count += in[2];
		struct index_node in_node;
		in_node.index = index;
		in_node.count = in[2];
		push_vector(&rn->indexes, &in_node);
		if (in[3] == 0)
			break;
		fsetpos(fp, &index_cur);
		fseek(fp, (in[3] - 1) * sizeof(size_t) * 4, SEEK_CUR);
	}
}

int main(int argc, char* argv[])
{
	if (argc <= 2)
		return 0;
	FILE *index_db = 0;
	char ab_path[1024];
	char index_db_path[1050];
	get_real(argv[1], ab_path);
	strcpy(index_db_path, ab_path);
	index_db = fopen(strcat(index_db_path, "/.index_db"), "rb");
	
	if (!index_db)
	{
		printf("the directory \"%s\" has not been indexed\n", ab_path);
		return -1;
	}
	
	merge_sort(argv + 2, argv + argc, voidpstrcmp, sizeof(char*));
	size_t cnt_index = 0;
	fread(&cnt_index, sizeof(size_t), 1, index_db);
	fseek(index_db, 2L + cnt_index * sizeof(size_t) * 4, SEEK_CUR);
	int letter = 0;
	size_t root = 0;
	string index = make_vector(char);
	struct vector indexes = make_vector(string);
	struct vector indexes_offset = make_vector(size_t);
	while ((letter = fgetc(index_db)) != (unsigned char)'\n')
	{
		if (letter != ' ')
			push_vector(&index, &letter);
		else
		{
			push_vector(&index, "");
			fread(&root, sizeof(size_t), 1, index_db);
			if (binary_search(argv + 2, argv + argc, &index.val, voidpstrcmp, sizeof(char**)))
			{
				push_vector(&indexes, &index);
				push_vector(&indexes_offset, &root);
			}

			index = make_vector(char);
			fseek(index_db, 1L, SEEK_CUR);
		}
	}
	
	fpos_t fn_pos;
	fgetpos(index_db, &fn_pos);

	struct hash_t result_t = make_hash_t(struct result_node);


	string *pindex = begin_vector(&indexes, string);
	size_t *pindex_off = begin_vector(&indexes_offset, size_t);
	for (size_t i = 0; i < indexes.size; ++i)
		get_result(*(pindex + i), *(pindex_off + i), &result_t, index_db, fn_pos);	 
	
	struct vector result_v = make_vector(struct result_node);
	string *key = 0;
	for (size_t i = 0; i < result_t.capacity; ++i)
		if ((key = get_p_vector(&result_t.key, i, string))->size != 0)
			push_vector(&result_v, get_hash_t(&result_t, *key, struct result_node));

	merge_sort(begin_vector(&result_v, struct result_node), end_vector(&result_v, struct result_node), &file_cnt_cmp, sizeof(struct result_node));
	merge_sort(begin_vector(&result_v, struct result_node), end_vector(&result_v, struct result_node), &index_cnt_cmp, sizeof(struct result_node));
	
	struct result_node *rn = 0;
	struct index_node *in_node = 0;
	printf("\nResult:\n\n");
	for (size_t i = 0; i < result_v.size; ++i)
	{
		rn = get_p_vector(&result_v, i, struct result_node);
		printf("In \"%s\"\n", rn->file_name.val);
		for (size_t j = 0; j < rn->indexes.size; ++j)
		{
			in_node = get_p_vector(&rn->indexes, j, struct index_node);
			printf("%s\n\toccurs %10lu time(s)\n", in_node->index.val, in_node->count);
		}
		printf("\n");
	}
	free_vector(&result_v);
	free_vector(&indexes);
	free_vector(&indexes_offset);
	free_string(&index);
	free_hash_t(&result_t);
	return 0;
}
