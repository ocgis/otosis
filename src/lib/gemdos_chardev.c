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
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#include "div.h"
#include "prototypes.h"
#include "gemdos.h"
#include "bios.h"
#include "toserrors.h"

#include "fd.h"

long generic_cout( int fd, int c )
{
  GETFD( fd );
  if (fd < 0)
	  return internal_Bconout( BIOSDEV(fd), c );
  else {
	  char cc = c;
	  write( fd, &cc, 1 );
	  return 0;
  }
}

#define CTRL(c)		((c) & 0x1f)

long generic_cin( int fd, int flags )
{
  long c;
  
  GETFD( fd );

  if (fd < 0) {
	  c = internal_Bconin( BIOSDEV(fd) );
	  if (flags & DO_ECHO) {
		  /* echo on same device as read from (if STDOUT is redirected...) */
		  internal_Bconout( BIOSDEV(fd), c );
	  }
	  if (flags & DO_CTRL) {
		  /* ^C exits program */
		  if ((c & 0xff) == CTRL('C'))
			  rexit( -32 );
		  /* ^S and ^Q should also be processed sometimes... */
	  }
  }
  else {
	  unsigned char cc;
	  if (read( fd, &cc, 1 ) != 1)
		  return 0;
	  c = cc;
  }
  return c;
}

long ioready( int fd, int mode )
{
  fd_set set;
  struct timeval tv;
  int rv;
  
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO( &set );
  FD_SET( fd, &set );

  rv = (mode == O_RDONLY) ? select( fd+1, &set, NULL, NULL, &tv )
	                      : select( fd+1, NULL, &set, NULL, &tv );
  if (rv == -1 ) {
	perror( "oTOSis internal error on select" );
    return 0; /* not ready... */
  }
  else
	return FD_ISSET( fd, &set ) ? -1 : 0;
}

long generic_instat( int fd )
{
  GETFD( fd );
  if (fd < 0)
	  return internal_Bconstat( BIOSDEV(fd) );
  else
	  return ioready( fd, O_RDONLY );
}

long generic_outstat( int fd )
{
  GETFD( fd );
  if (fd < 0)
	  return internal_Bcostat( BIOSDEV(fd) );
  else
	  return ioready( fd, O_WRONLY );
}

GEMDOSFUNC(Cconin)
{
  return generic_cin( GEMDOS_STDIN, DO_ECHO|DO_CTRL );
}

GEMDOSFUNC(Cconout)
{
  TOSARG(short,c);
  return generic_cout( GEMDOS_STDOUT, c );
}

GEMDOSFUNC(Cauxin)
{
  return generic_cin( GEMDOS_STDAUX, DO_NOTHING );
}

GEMDOSFUNC(Cauxout)
{
  TOSARG(short,c);
  return generic_cout( GEMDOS_STDAUX, c );
}

GEMDOSFUNC(Cprnout)
{
  TOSARG(short,c);
  return generic_cout( GEMDOS_STDPRN, c );
}

GEMDOSFUNC(Crawio)
{
  TOSARG(short,c);

  if (c == 0xff)
	  return generic_cin( GEMDOS_STDIN, DO_NOTHING );
  else
	  return generic_cout( GEMDOS_STDOUT, c );
}

GEMDOSFUNC(Crawcin)
{
  return generic_cin( GEMDOS_STDIN, DO_NOTHING );
}

GEMDOSFUNC(Cnecin)
{
  return generic_cin( GEMDOS_STDIN, DO_CTRL );
}

GEMDOSFUNC(Cconws)
{
  TOSARG(char *,str);
  int fd = GEMDOS_STDOUT;
  
  GETFD( fd );
  if (fd < 0) {
	  while( *str )
		  internal_Bconout( BIOSDEV(fd), *str++ );
	  return 0;
  }
  else {
	  write( fd, str, strlen(str) );
	  return 0;
  }
}

/* The real Cconrs has support for other control
   charcters than backspace, it will be implemented
   here too */
GEMDOSFUNC(Cconrs)
{
  TOSARG(unsigned char *,src);
  unsigned char *dest = src + 2;
  int max = *src;
  unsigned char ch;
  int pos;
  int fd = GEMDOS_STDIN;
  
  GETFD( fd );

  if (fd < 0) {
	  pos = gemdos_readstr( fd, dest, max );
  }
  else {
	  for( pos = 0; max-- > 0; ++pos, ++dest ) {
		  if (read( fd, dest, 1 ) != 1)
			  break;
#if 0
		  /* This is what the real GEMDOS would do: echo to handle 1, no
		   * matter what that is... I think this is bad for redirected
		   * channels, so it's commented out. */
		  generic_cout( GEMDOS_STDOUT, *dest );
#endif
		  if (*dest == '\r') {
			  /* Again real GEMDOS behaviour: After a CR, it unconditionally
			   * reads another char (should be LF, but who knows...) */
			  read( fd, &ch, 1 );
			  break;
		  }
	  }
  }
  src[1] = pos;
  return TOS_E_OK;
}

int gemdos_readstr( int fd, char *dest, int max )
{
	int pos, end_read;
	unsigned char ch;

	fd = BIOSDEV(fd);
	
	for( pos = 0, end_read = 0; pos < max && !end_read; ) {
		ch = internal_Bconin( fd );
		switch( ch ) {
	  	  case '\b':
			if (pos > 0) {
				internal_Bconout( fd, '\b' );
				internal_Bconout( fd, ' ' );
				internal_Bconout( fd, '\b' );
				pos--;
			}
			break;
			
	  	  case '\n':
		  case '\r':
			internal_Bconout( fd, '\r' );
			internal_Bconout( fd, '\n' );
			end_read = 1;
			break;
			
		  default:
			internal_Bconout( fd, ch );
			dest[pos++] = ch;
		} 
	} while( !end_read );

	return( pos );
}

GEMDOSFUNC(Cconis)
{
  return generic_instat( GEMDOS_STDIN );
}

GEMDOSFUNC(Cconos)
{
  return generic_outstat( GEMDOS_STDOUT );
}

GEMDOSFUNC(Cprnos)
{
  return generic_outstat( GEMDOS_STDPRN );
}

GEMDOSFUNC(Cauxis)
{
  return generic_instat( GEMDOS_STDAUX );
}

GEMDOSFUNC(Cauxos)
{
  return generic_outstat( GEMDOS_STDAUX );
}

/* rest of the functions */

GEMDOSFUNC(Tgetdate)
{
  struct tm *ti;
  time_t currtime;
  Date tdate;
  long ret;

  currtime = time( NULL );
  ti = localtime( &currtime );
  tdate.year = ti->tm_year - 80;
  tdate.month = ti->tm_mon + 1;
  tdate.day = ti->tm_mday;
  ret = *(short *)&tdate;
  return ret;
}

GEMDOS_UNIMP(Tsetdate);

GEMDOSFUNC(Tgettime)
{
  struct tm *ti;
  time_t currtime;
  Time ttime;
  long ret;

  currtime = time( NULL );
  ti = localtime( &currtime );
  ttime.hour = ti->tm_hour;
  ttime.minute = ti->tm_min;
  ttime.second = ti->tm_sec;
  ret = *(short *)&ttime;
  return ret;
}

GEMDOS_UNIMP(Tsettime);

GEMDOSFUNC(Sversion)
{
  /* Just return anything here... it's not really sure which GEMDOS version
   * we're emulating :-) */
  return 0x1700;
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
