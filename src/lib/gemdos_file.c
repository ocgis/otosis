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
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>
#include <time.h>

#include "memory.h"
#include "div.h"
#include "prototypes.h"
#include "gemdos.h"
#include "bios.h"
#include "toserrors.h"
#include "fd.h"

extern TosProgram *prog;
extern DirContext find_info;	/* Used for Fsfirst/Fsnext operation */

GEMDOSFUNC(Fsetdta)
{
  TOSARG(Dta *,dta);

  prog->dta = dta;
/* My documentation says nothing about the return value
   of Fsetdta, is this correct? */
/* ++roman: Fsetdta() is really void, so return anything... :-) */
  return TOS_E_OK;
}

GEMDOSFUNC(Fgetdta)
{
  return (UInt32)prog->dta;
}

/*
 *  Some notes about the filename emulation:
 *
 *  All input filename arguments goes through the
 *  function tos_to_unix() which transforms
 *  all backslashes to forward slashes and strips
 *  off any drive specifications if they exist.
 *  If one wants to implement the ability to map
 *  a drive to a specific directory, it should be
 *  done in this function.
 *
 *  Filenames that are sent back to the TOS-program
 *  goes through unix_to_tos() which changes
 *  all forward slashes to backslashes and makes sure
 *  that there are no filenames that violate the 8+3
 *  Mess-DOS naming standard.
 *
 *  These two functions can be found in the file
 *  file_emu.c
 *
 */

/*
 *  The file handles returned from open(2) are
 *  32 bit integers, but TOS uses only 16 bit
 *  file handles. but there shouldn't be any
 *  problems with that, as long as no more than
 *  64K files are opened
 *
 */

GEMDOSFUNC(Fcreate)
{
  TOSARG(char *,filename);
  TOSARG(SInt16,attr);
  SIGSTACK_STATIC char new_fname[ 1024 ];
  int fp, fd, mode;

/*
 *  Bits in the attribute argument:
 *  0 = read-only
 *  1 = hidden
 *  2 = system file
 *  3 = volume label
 *  4 = directory
 *  5 = archive
 *
 *  Current attribute processing:
 *   - hidden, system, and archive are completely ignored
 *   - label and dir return an error
 *   - read-only controls the w-bits in the access rights
 *
 */

  if (attr & (TOS_ATTRIB_VOLUME | TOS_ATTRIB_DIRECTORY))
	  return TOS_EACCDN;
  
  TOS_TO_UNIX( new_fname, filename );
  mode = (attr & TOS_ATTRIB_READONLY) ? 0444 : 0666;

  if ((fd = new_handle()) < 0)
	  return fd;
  
  if( (fp = open( new_fname, O_CREAT | O_WRONLY | O_TRUNC, mode )) == -1 ){
	FdMap[fd] = HANDLE_FREE;
    return translate_error( errno );
  }
  FdMap[fd] = fp;
  FdCnt[fp] = 1;
  return fd;
}

GEMDOSFUNC(Fopen)
{
  /* This function might contain a bug.
     should a file opened with Fopen for writing
     have its file pointer in the beginning or
     the end of the file? */
  /* ++roman: at the start :-) */

  TOSARG(char *,filename);
  TOSARG(SInt16,mode);
  SIGSTACK_STATIC char new_fname[ 1024 ];
  int omode;
  int fp, fd;

  /* Check for special names and return negative _16_ bit numbers */
  if (strcasecmp( filename, "con:" ) == 0)
	  return GEMDOS_DEV_CON & 0xffff;
  else if (strcasecmp( filename, "aux:" ) == 0)
	  return GEMDOS_DEV_AUX & 0xffff;
  else if (strcasecmp( filename, "prn:" ) == 0)
	  return GEMDOS_DEV_PRN & 0xffff;
  
  TOS_TO_UNIX( new_fname, filename );
  DDEBUG( "  opening file: %s, after: %s\n", filename, new_fname );
  DDEBUG( "  mode = %d\n", mode );
  switch( mode & 7 ) {
	  /* Never use O_CREATE with Fopen, the real one fails on non-existing
	   * files! */
	case 0:
	  omode = O_RDONLY;
	  break;
	case 1:
	  omode = O_WRONLY;
	  break;
	case 2:
	  omode = O_RDWR;
	  break;
	default:
	  return TOS_ERROR;
  }
  if( prog->emulate_mint ) {
	/*
	 *  It appears is bit 9 of the mode agrument is set,
	 *  the file will be created if it doesn't already
	 *  exist. None of my manuals have any documentation
	 *  on this.
	 */
	if( mode & 0x0200 ) {
	  omode |= O_CREAT;
	}
  }

  if ((fd = new_handle()) < 0)
	  return fd;
  if( (fp = open( new_fname, omode, 0 )) == -1 ){
    DDEBUG( "  open failed: %s\n", strerror( errno ) );
    return translate_error( errno );
  }
  FdMap[fd] = fp;
  FdCnt[fp] = 1;
  DDEBUG( "  Unix handle: %d GEMDOS handle: %d\n", fp, fd );
  return fd;
}

GEMDOSFUNC(Fclose)
{
  TOSARG(SInt16,fd);
  int map;
  
  if (IS_DEV_HANDLE(fd))
	  /* These cannot be closed... silently do nothing (like the real
	   * GEMDOS does) */
	  return TOS_E_OK;
  if (IS_FREE_HANDLE(FdMap[fd]))
	  return TOS_EIHNDL;

  map = FdMap[fd];
  FdMap[fd] = HANDLE_FREE;
  if (map >= 0) {
	  if (--FdCnt[map] == 0) {
		  if (close( map ))
			  return translate_error( errno );
	  }
  }
  return TOS_E_OK;
}

GEMDOSFUNC(Fread)
{
  TOSARG(SInt16,fd);
  TOSARG(UInt32,length);
  TOSARG(char *,buffer);
  int fp = fd, ret;

  GETFD( fp );
  if (fp < 0) {
	  /* for length 1, this is like Cconin(), else like Cconrs() */
	  if (length == 1) {
		  *buffer = generic_cin( fd, DO_ECHO|DO_CTRL );
		  return 1;
	  }
	  else
		  return( gemdos_readstr( fp, buffer, length ) );
  }

  if( (ret = read( fp, buffer, length )) == -1 ){
    int e = errno;
    DDEBUG( "  failed: %s\n", strerror( errno ) );
    errno = e;
    return translate_error( errno );
  }
  return ret;
}
  
GEMDOSFUNC(Fwrite)
{
  TOSARG(SInt16,fd);
  TOSARG(UInt32,length);
  TOSARG(char *,buffer);
  int ret, fp = fd;

  GETFD( fp );
  if (fp < 0) {
	  ret = length;
	  while( length-- )
		  internal_Bconout( BIOSDEV(fp), *buffer++ );
	  return ret;
  }

  if( (ret = write( fp, buffer, length )) == -1 ){
    DDEBUG( "  write failed: %s\n", strerror( errno ) );
    return translate_error( errno );
  }
  return ret;
}
  
GEMDOSFUNC(Fdelete)
{
  TOSARG(char *,filename);
  SIGSTACK_STATIC char new_fname[ 1024 ];

  TOS_TO_UNIX( new_fname, filename );
  if( unlink( new_fname ) == -1 ){
    return translate_error( errno );
  }
  return TOS_E_OK;
}

GEMDOSFUNC(Fseek)
{
  TOSARG(SInt32,offset);
  TOSARG(SInt16,fd);
  TOSARG(SInt16,mode);
  int nmode, fp = fd;
  UInt32 ret;

  GETFD( fp );
  if (fp < 0)
	  return TOS_EIHNDL;
  
  switch( mode ){
  case 0:
    nmode = SEEK_SET;
    break;
  case 1:
    nmode = SEEK_CUR;
    break;
  case 2:
    nmode = SEEK_END;
    break;
  default:
    return TOS_ERROR;
  }

  if( (ret = lseek( fp, offset, nmode )) == -1 ){
    return translate_error( errno );
  }
  return ret;
}

GEMDOSFUNC(Fattrib)
{
  TOSARG(char *,filename);
  TOSARG(SInt16,wflag);
  TOSARG(SInt16,attr);
  SIGSTACK_STATIC char new_fname[ 1024 ];
  struct stat st_buf;
  int w, new_mode;
  
  TOS_TO_UNIX( new_fname, filename );
  if (stat( new_fname, &st_buf ))
	  return translate_error( errno );

  if (!wflag) {
	  /*
	   * read attributes;
	   * we only return DIRECTORY and READONLY, the latter if we don't have
	   * write access to the file
	   */
	  attr = 0;
	  if (S_ISDIR( st_buf.st_mode )) {
        attr |= TOS_ATTRIB_DIRECTORY;
      }

	  if (getuid() == st_buf.st_uid)
		  w = st_buf.st_mode & S_IWUSR;
	  else if (getgid() == st_buf.st_gid)
		  w = st_buf.st_mode & S_IWGRP;
	  else
		  w = st_buf.st_mode & S_IWOTH;
	  if (!w)
		  attr |= TOS_ATTRIB_READONLY;

	  return attr;
  }
  else {
	  /*
	   * We only allow changing READONLY; DIRECTORY must be consistent with
	   * the file type. HIDDEN, SYSTEM and ARCHIVE are ignored, VOLUME returns
	   * an error
	   */
	  if (!!S_ISDIR(st_buf.st_mode) != !!(attr & TOS_ATTRIB_DIRECTORY) ||
		  (attr & TOS_ATTRIB_VOLUME))
		  return TOS_EACCDN;

	  new_mode = st_buf.st_mode;
	  if (attr & TOS_ATTRIB_READONLY)
		  new_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
	  else
		  /* we turn on ALL w-bits here... is that the right thing to do?? */
		  new_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
	  if (new_mode != st_buf.st_mode)
		  if (chmod( new_fname, new_mode ))
			  return translate_error( errno );

	  return TOS_E_OK;
  }
}

GEMDOSFUNC(Fdup)
{
	TOSARG(SInt16,fd);
	SInt32 fp = fd, fp2;

	if (!IS_STD_HANDLE(fp))
		return TOS_EIHNDL;

	GETFD( fp );
	if ((fp2 = new_handle()) < 0)
		return fp2;

	FdMap[fp2] = fp;
	if (fp >= 0)
		/* is a Unix fd, increment refcnt (device handles can be simply
		 * copied) */
		FdCnt[fp]++;
	return fp2;
}

GEMDOSFUNC(Fforce)
{
  TOSARG(SInt16,standh);
  TOSARG(SInt16,nonstandh);
  SInt32 stdh = standh, nstdh = nonstandh;
  SInt32 smap, nmap;
  
  if (!IS_STD_HANDLE(stdh) || IS_STD_HANDLE(nstdh))
	  return TOS_EIHNDL;

  if ((nstdh < 0 && !IS_DEV_HANDLE(nstdh)) ||
	  (nstdh >= 0 && IS_FREE_HANDLE(FdMap[nstdh])))
	  return TOS_EIHNDL;

  /* We're going to overwrite FdMap[stdh], so we're loosing that reference;
   * check for closing! */
  if ((smap = FdMap[stdh]) >= 0) {
	  if (--FdCnt[smap] == 0)
		  close( smap );
  }

  nmap = FdMap[nstdh];
  FdMap[stdh] = nmap;
  if (nmap >= 0)
	  FdCnt[nmap]++;

  return TOS_E_OK;
}

GEMDOSFUNC(Fsfirst)
{
  TOSARG(char *,filename);
  TOSARG(SInt16,mask);
  SIGSTACK_STATIC char tfname[ 1024 ], fname[ 1024 ];

  find_info.find_mask = mask;
  TOS_TO_UNIX( tfname, filename );
  get_dirname( fname, tfname );
  get_basename( find_info.file_mask, tfname );
  if( find_info.dir_pointer != NULL ){
    closedir( find_info.dir_pointer );
  }
  if( (find_info.dir_pointer = opendir( fname )) == NULL ){
    return translate_error( errno );
  }
  return gemdos_Fsnext( NULL );
}

GEMDOSFUNC(Fsnext)
{
  struct dirent *e;
  SIGSTACK_STATIC char fname[ 14 ];
  int a = 0;
  struct stat fstat;

  do{
    if( (e = readdir( find_info.dir_pointer )) == NULL ){
      return TOS_ENMFIL;
    }

    stat( e->d_name, &fstat );

    if( S_ISDIR( fstat.st_mode ) ){
      a |= TOS_ATTRIB_DIRECTORY;
    }

    /*  BUG: this routine should check the group file too */
    if( fstat.st_uid == getuid() ){
      if( (fstat.st_mode & S_IWUSR) == 0 ){
		a |= TOS_ATTRIB_READONLY;
      }
    }
    else if( fstat.st_gid == getgid() ){
      if( (fstat.st_mode & S_IWGRP) == 0 ){
		a |= TOS_ATTRIB_READONLY;
      }
    }
    else{
      if( (fstat.st_mode & S_IWOTH) == 0 ){
		a |= TOS_ATTRIB_READONLY;
      }
    }
  }while( ((a & TOS_ATTRIB_READONLY) && !(find_info.find_mask & TOS_ATTRIB_READONLY)) ||
	 (S_ISDIR( fstat.st_mode ) && !(find_info.find_mask & TOS_ATTRIB_DIRECTORY)) ||
	 (fnmatch( find_info.file_mask, e->d_name, FNM_NOESCAPE ) != 0) );

  prog->dta->d_attrib = a;
  prog->dta->d_time = 0;
  prog->dta->d_date = 0;
  prog->dta->d_length = fstat.st_size;
  unix_to_tos_file_short( fname, e->d_name );
  strcpy( prog->dta->d_fname, fname );

  return TOS_E_OK;
}

GEMDOSFUNC(Frename)
{
  TOSARG(SInt16,dummy);
  TOSARG(char *,src);
  TOSARG(char *,dest);
  SIGSTACK_STATIC char new_src[ 1024 ], new_dest[ 1024 ];

  ARG_USED(dummy);
  
  TOS_TO_UNIX( new_src, src );
  TOS_TO_UNIX( new_dest, dest );
  if( rename( new_src, new_dest ) == -1 ){
    return translate_error( errno );
  }
  return TOS_E_OK;
}

GEMDOSFUNC(Fdatime)
{
  TOSARG(Datetime *,timeptr);
  TOSARG(SInt16,fd);
  TOSARG(SInt16,flag);
  int handle = fd;
  struct stat buf;
  struct tm *ltime;

  GETFD( handle );
  if (handle < 0)
	  return TOS_EIHNDL;
  
  if( flag == 0 ){
    if( fstat( handle, &buf ) == -1 ){
      return translate_error( errno );
    }
    ltime = localtime( &buf.st_mtime );
    timeptr->hour = ltime->tm_hour;
    timeptr->minute = ltime->tm_min;
    timeptr->second = ltime->tm_sec / 2;
    timeptr->year = ltime->tm_year - 80;
    timeptr->month = ltime->tm_mon + 1;
    timeptr->day = ltime->tm_mday;
    return TOS_E_OK;
  }
  else if( flag == 1 ){
/* Currently don't allow changing of the file's date
 * Basically this is possible, but Unix utime() needs a file name, and we got
 * a handle here... :-( Would need some reverse translation, or kinda
 * futime()...
 */
    return TOS_EBADRQ;
  }
  else{
    return TOS_ERANGE;
  }
}

GEMDOS_UNIMP(Flock);

GEMDOSFUNC(Dsetdrv)
{
  TOSARG(SInt16,drive);
  int old_drive = prog->curdrv;

  CHECK_DRV( drive );
  prog->curdrv = drive;
  return old_drive;
}

GEMDOSFUNC(Dgetdrv)
{
  return prog->curdrv;
}

GEMDOSFUNC(Dfree)
{
  TOSARG(Diskinfo *,buf);
  TOSARG(SInt16,drive);

  ARG_USED(drive);
  buf->b_free = 1000;
  buf->b_total = 2000;
  buf->b_secsize = 512;
  buf->b_clsize = 4;
  return TOS_E_OK;
}

GEMDOSFUNC(Dcreate)
{
  TOSARG(char *,name);
  SIGSTACK_STATIC char new_fname[ 1024 ];

  TOS_TO_UNIX( new_fname, name );
  if( mkdir( new_fname, 0755 ) == -1 ){
    return translate_error( errno );
  }
  return TOS_E_OK;
}

GEMDOSFUNC(Ddelete)
{
  TOSARG(char *,name);
  SIGSTACK_STATIC char new_fname[ 1024 ];

  TOS_TO_UNIX( new_fname, name );
  if( rmdir( new_fname ) == -1 ){
    return translate_error( errno );
  }
  return TOS_E_OK;
}

/*
 *  This function is a bit more difficult to do than one
 *  might think. It is not as simple as doing a tos_to_unix
 *  and then a UNIX chdir(2) to that directory. We must
 *  keep track of the different default directories
 *  and resolve . and .. entries in the path.
 *  The reolving is done through the function
 *  resolve_tos_file, that function does not check
 *  that the directories actually exist, so we
 *  first resolve the TOS path, convert it to UNIX
 *  and try to chdir(2) to it. If this succeeds
 *  we can assume that the path was legal.
 *
 */

GEMDOSFUNC(Dsetpath)
{
  TOSARG(char *,path);
  SIGSTACK_STATIC char src[ 1024 ];
  SIGSTACK_STATIC char src2[ 1024 ];
  SIGSTACK_STATIC char dest[ 1024 ];
  SIGSTACK_STATIC char dest2[ 1024 ];
  int drive, drvchar;

  if( strlen( path ) >= 2 && (drvchar = path[ 1 ]) == ':' ) {
	drive = drvchar - 'a';
	CHECK_DRV( drive );
	path += 2;
  }
  else {
	drive = prog->curdrv;
  }

  if( *path != '\\' ) {
	sprintf( src, "%s\\%s", prog->curdir[ drive ], path );
  }
  else {
	strcpy( src, path );
  }

  DDEBUG( "  resolving: %s\n", src );
  if( !resolve_tos_file( dest, src ) ) {
	return TOS_EPTHNF;
  }
  DDEBUG( "  after resolve: %s\n", dest );
  sprintf( src2, "%c:%s", drive + 'a', src );
  DDEBUG( "  converting: %s\n", src2 );
  TOS_TO_UNIX( dest2, src2 );
  DDEBUG( "  after convert: %s\n", dest2 );
  if( chdir( dest2 ) != 0 ) {
	return TOS_EPTHNF;
  }
  DDEBUG( "  chdir succeded\n" );
  strcpy( prog->curdir[ drive ], src );
  return TOS_E_OK;
}

GEMDOSFUNC(Dgetpath)
{
  TOSARG(char *,buf);
  TOSARG(SInt16,drive);

  if( drive == 0 ) {
	drive = prog->curdrv;
  }
  else {
	drive--;
  }
  
  CHECK_DRV( drive );
  strcpy( buf, prog->curdir[ drive ] );
  DDEBUG( "  returning: %s\n", buf );

  return TOS_E_OK;
}


/* ---------------------------------------------------------------------- */
/*								Handle Mapping							  */

/*
 * getfd() is the central function for the GEMDOS -> Unix handle mapping.
 * It modifies the GEMDOS handle passed in *fd to become either a standard
 * Unix fd (>= 0) or a GEMDOS device handle (< 0). If the mapping fails, the
 * function returns False and the caller should (most probably) return
 * TOS_EIHNDL. If everything is ok, getfd() returns True (1).
 */

SInt32 getfd( SInt32 *fd )

{
	if (IS_DEV_HANDLE(*fd))
		/* is a device handle: leave unchanged and return ok */
		return 1;

	*fd = FdMap[*fd];
	if (IS_FREE_HANDLE(*fd))
		/* handle is not opened! */
		return 0;
	else
		/* handle is ok, return either device# or Unix fd */
		return 1;
}


/*
 * get a fresh GEMDOS handle for a Unix fd
 * returns new GEMDOS handle, or < 0 for error
 */

SInt32 new_handle( void )

{	int	i;
	
	for( i = GEMDOS_FIRST_NONSTD; i < MAX_GEMDOS_FD; ++i )
		if (IS_FREE_HANDLE(FdMap[i])) break;
	if (i == MAX_GEMDOS_FD)
		/* sorry, no more free handles */
		return TOS_ENHNDL;
	return i;
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
