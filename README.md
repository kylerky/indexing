# indexing
A programme for indexing text files (MIME type: text/*) in Linux

Compile the index_builder programme with

gcc -lmagic path.c str.c vector.c hash_t.c index_builder.c -o index_builder

and the search promgramme with

gcc path.c str.c vector.c hash_t.c merge_sort.c insertion_sort.c search.c -o search


# HOW-TOs
first you need to index a directory with

$ ./index_builder <path>

then you can search the directory with some key words

$ ./search <path> <key1[, key2[, ...]]>
