/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
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

#include <div.h>
#include <prototypes.h>
#include <gemdos.h>
#include <toserrors.h>
#include <option.h>

/*
 *  Some kind of memory allocation control should be possible,
 *  i.e. it should be possible to specify the maximum memory
 *  usage of a TOS-program, right now Malloc( -1 ) always
 *  returns a static value
 *
 */

/*
 *  TOS seems to be very lax about illegal Mfree()'s.
 *  The libc free() call crashes with a SEGV if the memory
 *  segment isn't previously allocated. In order to emulate
 *  this we need to keep track of all malloc()'ed areas.
 *
 */

extern ulong *malloc_regions;
extern int malloc_regions_size;

GEMDOS_UNIMP(Maddalt);

GEMDOSFUNC(Mxalloc)
{
#if 0
  TOSARG(ulong,size);
  TOSARG(short,mode);
#endif
  
/* For now, Mxalloc is just a copy of Malloc,
   if anybody has any ideas on how this might be
   implemented please mail me about it */

  return gemdos_Malloc( _args );
}

GEMDOSFUNC(Malloc)
{
  TOSARG(ulong,size);
  ulong addr;
  ulong *w;

  if( size == -1 )  return Opt_mem_free*1024;
  addr = (ulong)malloc( size );
  for( w = malloc_regions ; *w && *w != -1 ; w++ );
  if( *w == 0 ) {
    if( w - malloc_regions >= malloc_regions_size ) {
      malloc_regions = myrealloc( malloc_regions, sizeof( ulong ) *
				 (malloc_regions_size + 10) );
      malloc_regions_size += 10;
    }
    w[ 1 ] = 0;
  }
  *w = addr;
  return addr;
}

GEMDOSFUNC(Mfree)
{
  TOSARG(void *,addr);
  ulong *w;

/* Mfree should return a negative error code if it fails,
   does anybody know what error codes Free might return? */

  for( w = malloc_regions ; *w && *w != (ulong)addr ; w++ );
  if( *w ) {
    *w = -1;
    free( addr );
    return TOS_E_OK;
  }
  else {
    return TOS_EIMBA;
  }
}

GEMDOSFUNC(Mshrink)
{
  TOSARG(void *,mem);
  TOSARG(ulong,size);

  ARG_USED(mem);
  ARG_USED(size);
/* I can't use realloc here, because it is allowed to move
   the memory, and TOS-programs doesn't like that.
   Because of this, I don't do anything here */
  return TOS_E_OK;
}

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
