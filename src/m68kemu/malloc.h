#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>

void *xmalloc( size_t );
void *xrealloc( void *, size_t );
char *xstrdup( char * );

#endif
