/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "div.h"
#include "prototypes.h"

void *mymalloc( size_t size )
{
  void *mem;

  if( (mem = malloc( size )) == NULL ){
    fprintf( stderr, "Could not allocate %d bytes\n", size );
    rexit( 1 );
  }

  return mem;
}

void *myrealloc( void *mem, size_t size )
{
  void *mem2;

  if( (mem2 = realloc( mem, size )) == NULL ){
    fprintf( stderr, "Could not reallocate %d bytes\n", size );
    rexit( 1 );
  }

  return mem2;
}

char *mystrdup( const char *str )
{
  char *str2;

  if( (str2 = malloc( strlen( str ) + 1 )) == NULL ) {
    fprintf( stderr, "Could not allocate %d bytes for string\n",
	    strlen( str ) + 1 );
    rexit( 1 );
  }
  strcpy( str2, str );
  return str2;
}

/* Local Variables:              */
/* tab-width: 8                  */
/* End:                          */
