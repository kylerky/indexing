#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <magic.h>
#include <dirent.h>
#include "hash_t.h"
#include "path.h"

static FILE *index_db = 0;
struct offset_node
{
	size_t root;
	size_t cur;
};

static struct hash_t offset = {};

static size_t foffset = 0;
static size_t ioffset = 0;
static struct vector fn = {};
void build_index(char *file)
{
	struct hash_t cnt = make_hash_t(size_t);
	FILE *fp = fopen(file, "r");
	unsigned char word[1024] = "";
	int letter;
	size_t i = 0;
	while ((letter = fgetc(fp)) != EOF)
	{
		if((letter >= (unsigned char)'0' && letter <= (unsigned char)'9') || (letter >= (unsigned char)'a' && letter <= (unsigned char)'z') || (letter >= (unsigned char)'A' && letter <= (unsigned char)'Z') || letter == (unsigned char)'_')
		{
			word[i++] = letter;
		}
		else if(i > 0)
		{
			word[i] = '\0';
			size_t *pcnt = 0;
			if (!(pcnt = get_hash_t(&cnt, make_string((char*)word), size_t)))
			{
				size_t tmp = 0;
				add_hash_t(&cnt, make_string((char*)word), &tmp);
				pcnt = get_hash_t(&cnt, make_string((char*)word), size_t);
			}
			++*pcnt;
			word[0] = '\0';
			i = 0;
		}
	}
	
	for (size_t j = 0; j < strlen(file); ++j)
		push_vector(&fn, file + j);
	string *key = 0;
	for (size_t j = 0; j < cnt.capacity; ++j)
	{
		key = get_p_vector(&(cnt.key), j, string);
		if (key->size != 0)
		{
			struct offset_node *index_off = 0;
			if (!(index_off = get_hash_t(&offset, *key, struct offset_node)))
			{
				struct offset_node ontmp = {ioffset, ioffset};
				add_hash_t(&offset, make_string(key->val), &ontmp);
			}
			else
			{
				fseek(index_db, 1 + (index_off->cur * 4 + 4) * sizeof(size_t), SEEK_SET);
				size_t rel_ioffset = ioffset - index_off->cur;
				fwrite(&rel_ioffset, sizeof(size_t), 1, index_db);
				index_off->cur = ioffset;
				fseek(index_db, 0L, SEEK_END);
			}
			size_t out[4];
			out[0] = foffset;
			out[1] = strlen(file);
			out[2] = *get_hash_t(&cnt, *key, size_t);
			out[3] = 0;
			fwrite(out, sizeof(size_t), 4, index_db);
			++ioffset;
		}
	}
	foffset += strlen(file);
	for (size_t j = 0; j < cnt.capacity; ++j)
		if ((key = get_p_vector(&(cnt.key), j, string)) && key->val)
			printf("%s occurs %lu time(s)\n", key->val, *get_hash_t(&cnt, *key, size_t));
	printf("\n");
	
	free_hash_t(&cnt);
}

void dig(char *directory)
{
	DIR *dir = 0;
	dir = opendir(directory);
	if (dir)
	{
		struct dirent *subdir = 0;
		while ((subdir = readdir(dir)))
		{
			char str[strlen(directory) + strlen((*subdir).d_name) + 1];
			strcpy(str, directory);
			strcat(str, "/");
			strcat(str, (*subdir).d_name);
			printf("name: %20s type: %d\n", str, (*subdir).d_type);
			if ((*subdir).d_type == DT_DIR && strcmp((*subdir).d_name, "..") && strcmp((*subdir).d_name, "."))
				dig(str);
			else if(subdir->d_type == DT_REG && strcmp(subdir->d_name, ".index_db") != 0)
			{
				const char *mime = 0;
				magic_t magic;
				magic = magic_open(MAGIC_MIME_TYPE);
				magic_load(magic, NULL);
				magic_compile(magic, NULL);
				mime = magic_file(magic, str);
				char type[6];
				if (strlen(mime) > 5)
				{
					strncpy(type, mime, 5);
					type[5] = '\0';
					if (strcmp(type, "text/") == 0)
						build_index(str);
				}
			}

		}
	}
	closedir(dir);
}



int main(int argc, char* argv[])
{
	if (argc <= 1)
		return 0;
	char ab_path[1024];
	char index_db_path[1050];
	get_real(argv[1], ab_path);
	strcpy(index_db_path, ab_path);
	index_db = fopen(strcat(index_db_path, "/.index_db"), "wb");
	fwrite(&ioffset, sizeof(size_t), 1, index_db);
	fputs("\n", index_db);
	fn = make_vector(char);
	offset = make_hash_t(struct offset_node);
	dig(ab_path);
	fputs("\n", index_db);
	string *key = 0;
	for (size_t j = 0; j < offset.capacity; ++j)
	{
		key = get_p_vector(&offset.key, j, string);
		if (key->size != 0)
		{
			fputs(key->val, index_db);
			fputs(" ", index_db);
			size_t root = get_hash_t(&offset, *key, struct offset_node)->root;
			fwrite(&root, sizeof(size_t), 1, index_db);
			fputs(" ", index_db);
		}
	}
	fputs("\n", index_db);
	push_vector(&fn, "");
	fputs(fn.val, index_db);
	fseek(index_db, 0L, SEEK_SET);
	printf("%lu\n", ioffset);
	fwrite(&ioffset, sizeof(size_t), 1, index_db);
	
	free_hash_t(&offset);
	fclose(index_db);
	return 0;
}
