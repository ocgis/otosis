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
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"


TosProgram * prog = NULL;


/*
 *  translate_error
 *  This function translates UNIX error code into
 *  the corresponding TOS error code
 *  if there is no apropriate error TOS_ERROR
 *  is returned
 *
 */

int translate_error( int err )
{
  switch( err ){
  case EPERM:
    return TOS_EACCDN;
  case ENOENT:
    return TOS_EFILNF;
  case ENXIO:
    return TOS_EUNDEV;
  case EBADF:
    return TOS_EIHNDL;
  case ENOMEM:
    return TOS_EINSMEM;
  case EACCES:
    return TOS_EACCDN;
  case ENODEV:
    return TOS_EUNDEV;
  case ERANGE:
  case ENAMETOOLONG:
    return TOS_ERANGE;
  case ELOOP:
    return TOS_ELOOP;
  case ESPIPE:
    /* can happen for seeks on non-files, GEMDOS would return EIHDNL */
    return TOS_EIHNDL;
  default:
    DDEBUG( "Cannot translate Unix errno %d (%s)\n", err, strerror(err) );
    return TOS_ERROR;
  }
}




/*
 *  Notes about drive mapping and file name conversion:
 *
 *  These are the different functions available to
 *  convert between UNIX and TOS filenames.
 *
 *  tos_to_unix
 *      This function converts a TOS filename to
 *      a UNIX filename. This function actually
 *      calls tos_to_unix short or tos_to_unix_long
 *      depending on weither the TOS program is running
 *      in the MiNT domain or not.
 *
 *  unix_to_tos_path
 *      Convert a UNIX fully qualified filename
 *      to a TOS filename. Depending on the MiNT domain
 *      this function calls tos_to_unix_path_short
 *      or tos_to_unix_path_long.
 *
 *  unix_to_tos_file
 *      Convert a UNIX filename to a TOS filename.
 *      Depending on the MiNT domain this function
 *      calls tos_to_unix_file_short or
 *      tos_to_unix_file_long.
 *
 */

/*
 *  The drive mapping is implemented as follows:
 *
 *  In TOS every drive has it's own current directory.
 *  This is emulated using an array of TOS paths,
 *  one for each drive. There are three different
 *  actions that can be taken that involves drives:
 *
 *  Specifying a file
 *      Fopen, Fdelete, etc.
 *
 *  Changing directory
 *      Dsetpath
 *
 *  Getting the current directory
 *      Dgetpath
 *
 *  When a file is specified (Fopen etc.) the emulation
 *  routine calls tos_to_unix to convert the file to
 *  a UNIX file name. In this case, there are a couple
 *  of different possibilities:
 *
 *    Absolute directory without drive ("\foo\bar.txt")
 *    Relative directory without drive ("foo\bar.txt")
 *    Relative directory with drive ("c:foo\bar.txt")
 *    Absolute directory with drive ("c:\foo\bar.txt")
 *
 *  For the first example the drive map for the current drive
 *  is added. The second adds the drive map for the current
 *  drive plus the current dir for the current drive.
 *  The third and fourth examples wirk just like the first
 *  two except that they add the drive map and current dir
 *  for a different drive.
 *
 */


int tos_to_unix( char *dest, char *src )
{
  int drive;
  int is_relative = 1;
  char tmp_dest[ 1024 ], newsrc[ 1024 ];
  char *newsrc_ptr;

  newsrc_ptr = src;


  drive = prog->curdrv;
  if( strlen( src ) >= 2 ) {
    if( src[ 1 ] == ':' ) {
      drive = tolower( *src ) - 'a';
      CHECK_DRV( drive );
      newsrc_ptr += 2;
    }
  }
  if( *newsrc_ptr == '\\' ) {
    is_relative = 0;
  }

  *newsrc = 0;
  if( is_relative ) {
    strcpy( newsrc, prog->curdir[ drive ] );
    strcat( newsrc, "/" );
  }
  strcat( newsrc, newsrc_ptr );

  if( prog->domain == 1 ) {
    tos_to_unix_long( tmp_dest, newsrc );
  }
  else {
    tos_to_unix_short( tmp_dest, newsrc );
  }

  sprintf( dest, "%s/%s", prog->drive_map[ drive ], tmp_dest );

  return 0;
}

void unix_to_tos_path( char *dest, char *src )
{
  if( prog->domain == 1 ) {
    unix_to_tos_long( dest, src );
  }
  else {
    unix_to_tos_path_short( dest, src );
  }
}

void unix_to_tos_file( char *dest, char *src )
{
  if( prog->domain == 1 ) {
    unix_to_tos_long( dest, src );
  }
  else {
    unix_to_tos_file_short( dest, src );
  }
}

/*
 *  Translates all characters to lower case,
 *  changes all backslashes to forward slashes
 *  and strips off any drive specification
 *
 */

/*
 *  BUG: no checking is performed on the lenght
 *  of the destination file name
 *  it should be limited by MAXPATHLEN
 *
 */

void tos_to_unix_short( char *dest, char *src )
{
  /* This routine can be improved very much */

  /* If the filename is "*.*", then the application really wants
   * all files, so it's translated to "*" */
  if( strcmp( src, "*.*" ) == 0 ){
    strcpy( dest, "*" );
    return;
  }

  while( *src != 0 ){
    if( *src == '\\' ){
      *dest++ = '/';
    }
    else if( isalpha( *src ) && isupper( *src ) ){
      *dest++ = tolower( *src );
    }
    else{
      *dest++ = *src;
    }
    src++;
  }
  *dest = 0;
}

/*
 *  Fix the output filename to make it compliant
 *  with the Mess-DOS 8+3 naming convention.
 *  also change all forward slashes to backslashes.
 *
 */

void unix_to_tos_path_short( char *dest, char *src )
{
  char *start = src;
  char *end;
  char tmp_in[ 256 ], tmp_out[ 256 ];
  int c;

/* if the pathname begins with a slash, it has to be
 * converted explictly */
  if( *src == '/' ){
    *(dest++) = '\\';
    start++;
  }

/* check for null pathname */
  if( *start == 0 ){
    *dest = 0;
    return;
  }

/* find each file name and convert it */
  do{
    end = start + 1;
    while( *end != '/' && *end != 0 )  end++;
    strncpy( tmp_in, start, end - start );
    tmp_in[ end - start ] = 0;
    unix_to_tos_file_short( tmp_out, tmp_in );
    for( c = 0 ; c < strlen( tmp_out ) ; c++ ){
      *(dest++) = tmp_out[ c ];
    }
    while( *end == '/' ){
      *(dest++) = '\\';
      end++;
    }
    start = end;
  }while( *end != 0 );
  *(dest++) = 0;
}

/*
 *  Like the above but only return the file name
 *
 */

void unix_to_tos_file_short( char *dest, char *src )
{
  char *chptr = src;
  int c;

  while( *chptr != '.' && *chptr != 0 && chptr - src < 8 ){
    if( islower( *chptr ) ){
      *(dest++) = toupper( *(chptr++) );
    }
    else{
      *(dest++) = *(chptr++);
    }
  }
  if( *chptr == 0 ){
    *(dest++) = 0;
    return;
  }
  chptr = src + strlen( src ) - 1;
  while( *chptr != '.' && chptr > src ){
    chptr--;
  }
  if( chptr == src ){
    *(dest++) = 0;
    return;
  }
  *(dest++) = '.';
  chptr++;
  for( c = 0 ; c < 3 && *chptr != 0 ; c++ ){
    if( islower( *chptr ) ){
      *(dest++) = toupper( *(chptr++) );
    }
    else{
      *(dest++) = *(chptr++);
    }
  }
  *dest = 0;
}

void get_basename( char *dest, char *src )
{
  char *new_src = src + strlen( src ) - 1;
  char *end;

  if( *src == 0 ){
    *dest = 0;
    return;
  }
  if( strcmp( src, "/" ) == 0 ){
    strcpy( dest, "/" );
    return;
  }
  while( *new_src == '/' && new_src != src )  new_src--;
  end = new_src + 1;
  while( *new_src != '/' && new_src != src )  new_src--;
  if( *new_src == '/' )  new_src++;
  strncpy( dest, new_src, end - new_src );
  dest[ end - new_src ] = 0;
}

/*
 *  This is a long filenames version of unix_to_tos
 *  it is much simpler since the only thing needed to do
 *  is to convert the slashes to backslashes
 */
void tos_to_unix_long( char *dest, char *src )
{
  while( *src ) {
    if( *src == '\\' ) {
      *dest++ = '/';
      src++;
    }
    else {
      *dest++ = *src++;
    }
  }
  *dest = 0;
}

void unix_to_tos_long( char *dest, char *src )
{
  while( *src ) {
    if( *src == '/' ) {
      *dest++ = '\\';
      src++;
    }
    else {
      *dest++ = *src++;
    }
  }
  *dest = 0;
}

void get_dirname( char *dest, char *src )
{
  char *new_src = src + strlen( src ) - 1;

  if( *src == 0 ){
    strcpy( dest, "." );
    return;
  }
  if( strcmp( src, "/" ) == 0 ){
    strcpy( dest, "/" );
    return;
  }

  if( *new_src != '/' ){
    while( *new_src != '/' && new_src != src )  new_src--;
    if( new_src == src ){
      strcpy( dest, "." );
      return;
    }
  }
  while( *new_src == '/' && new_src != src )  new_src--;
  if( new_src == src ){
    strcpy( dest, "/" );
    return;
  }
  new_src++;
  strncpy( dest, src, new_src - src );
  dest[ new_src - src ] = 0;
}

void unix_time_to_tos( short *dest_time, short *dest_date, time_t src_time )
{
  struct tm *ltime;
  Time ttime;
  Date tdate;

  ltime = localtime( &src_time );
  ttime.hour = ltime->tm_hour;
  ttime.minute = ltime->tm_min;
  ttime.second = ltime->tm_sec / 2;
  tdate.year = ltime->tm_year - 80;
  tdate.month = ltime->tm_mon + 1;
  tdate.day = ltime->tm_mday;

  *dest_time = *(short *)&ttime;
  *dest_date = *(short *)&tdate;
}


int resolve_tos_file( char *dest, char *src )
{
  int depth = 0;
  char *head, *tail;
  char file[ 1024 ];
  char *w;

  if( *src != '\\' ) {
    printf( "Something wrong here! First character of the pathname\n" );
    printf( "must be a backslash.\n" );
    exit( 1 );
  }

  dest[ 0 ] = 0;

  head = src + 1;
  while( *head ) {
    tail = head;

    while( *head == '\\' ) {
      head++;
      tail++;
    }
    if( *head == 0 ) {
      strcat( dest, "\\" );
      return 1;
    }
    while( *head != 0 && *head != '\\' )  head++;
    strncpy( file, tail, head - tail );
    file[ head - tail ] = 0;
    if( strcmp( file, ".." ) == 0 ) {
      if( --depth < 0 ) {
	return 0;
      }
      /* backtrack to the last backslash */
      for( w = dest + strlen( dest ) ; w > dest && *w != '\\' ; w-- );
      *w = 0;
    }
    else if( strcmp( file, "." ) == 0 ) {
      printf( "found . in filename\n" );
    }
    else {
      depth++;
      strcat( dest, "\\" );
      strcat( dest, file );
    }
  }

  if( dest[ 0 ] == 0 ) {
    strcpy( dest, "\\" );
  }

  return 1;
}


/* Local Variables:              */
/* tab-width: 8                  */
/* End:                          */
