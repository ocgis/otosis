#include <stdio.h>
#include <string.h>

#include "malloc.h"

void *xmalloc( size_t size )
{
  void *ret;

  if( (ret = malloc( size )) == NULL ) {
    fprintf( stderr, "Can't allocate %d bytes\n", size );
    exit( 1 );
  }

  return ret;
}


void *xrealloc( void *mem, size_t size )
{
  void *ret;

  if( (ret = realloc( mem, size )) == NULL ) {
    fprintf( stderr, "Can't reallocate %d bytes\n", size );
    exit( 1 );
  }

  return ret;
}


char *xstrdup( char *str )
{
  char *ret;

  ret = xmalloc( strlen( str ) );
  strcpy( ret, str );
  return ret;
}
