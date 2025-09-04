#ifndef malloc.h
#define malloc.h

#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);

#endif