#include "str.h"

string make_string(char *arr)
{
	string str = make_vector(char);
	size_t len = strlen(arr);
	for (size_t i = 0; i < len; ++i)
		push_vector(&str, &arr[i]);
	push_vector(&str, "");
	return str;
}
