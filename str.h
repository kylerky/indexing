#ifndef STR_H
#define STR_H
#include "vector.h"
#include <string.h>
#define free_string(STR) free_vector(STR)
typedef struct vector string;

string make_string(char *arr);
#endif
