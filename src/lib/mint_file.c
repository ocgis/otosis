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
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "gemdos.h"
#include "mint.h"
#include "fd.h"

void stat_to_xattr( Xattr *, struct stat * );

extern TosProgram *prog;

MINTFUNC(Finstat)
{
  TOSARG(short,handle);

  return generic_instat( handle );
}

MINTFUNC(Foutstat)
{
  TOSARG(short,handle);

  return generic_outstat( handle );
}

MINTFUNC(Fchown)
{
  TOSARG(char *,filename);
  TOSARG(short,uid);
  TOSARG(short,gid);
  static char new_fname[ 1024 ];

  TOS_TO_UNIX( new_fname, filename );
  if( chown( new_fname, uid, gid ) == -1 ) {
    return translate_error( errno );
  }
  return 0;
}

MINTFUNC(Fchmod)
{
  TOSARG(char *,filename);
  TOSARG(short,mode);
  static char new_fname[ 1024 ];
  int newmode = 0;

  TOS_TO_UNIX( new_fname, filename );

  if( mode & TOS_S_IRUSR )  newmode |= S_IRUSR;
  if( mode & TOS_S_IWUSR )  newmode |= S_IWUSR;
  if( mode & TOS_S_IXUSR )  newmode |= S_IXUSR;
  if( mode & TOS_S_IRGRP )  newmode |= S_IRGRP;
  if( mode & TOS_S_IWGRP )  newmode |= S_IWGRP;
  if( mode & TOS_S_IXGRP )  newmode |= S_IXGRP;
  if( mode & TOS_S_IROTH )  newmode |= S_IROTH;
  if( mode & TOS_S_IWOTH )  newmode |= S_IWOTH;
  if( mode & TOS_S_IXOTH )  newmode |= S_IXOTH;

  if( chmod( new_fname, newmode ) == -1 ) {
    return translate_error( errno );
  }
  return 0;
}

MINTFUNC(Dgetcwd)
{
  TOSARG(char *,path);
  TOSARG(short,drive);
  TOSARG(short,size);

  ARG_USED(drive);
  
  if( drive == 0 ) {
    drive = prog->curdrv;
  }
  else {
    drive--;
  }
  CHECK_DRV( drive );
  if( strlen( prog->curdir[ drive ]) > size - 1 ) {
    return TOS_ERANGE;
  }
  strcpy( path, prog->curdir[ drive ] );
  DDEBUG( "  returning: %s\n", path );
  return TOS_E_OK;
}

MINTFUNC(Fxattr)
{
  TOSARG(short,flag);
  TOSARG(char *,name);
  TOSARG(Xattr *,attr);
  struct stat buf;
  static char new_fname[ 1024 ];
  int retcode;
  int ret;

  TOS_TO_UNIX( new_fname, name );
  DDEBUG( "  name after: %s\n", new_fname );
  if( flag == 0 )	ret = stat( new_fname, &buf );
  else if( flag == 1 )	ret = lstat( new_fname, &buf );
  else			return TOS_EBADRQ;
  if( ret == -1 ) {
    retcode = errno;
    DDEBUG( "  stat failed: %s\n", strerror( errno ) );
    return translate_error( retcode );
  }
  DDEBUG( "  stat succeeded size = %ld\n", (long)buf.st_size );

  stat_to_xattr( attr, &buf );

  return TOS_E_OK;
}

MINTFUNC(Freadlink)
{
  TOSARG(short,bufsiz);
  TOSARG(char *,buf);
  TOSARG(char *,name);
  static char buf2[ MAXPATHLEN + 1];
  static char name2[ MAXPATHLEN + 1];
  int len;

  TOS_TO_UNIX( name2, name );
  if( (len = readlink( name2, buf2, MAXPATHLEN )) == -1 ) {
    return translate_error( errno );
  }
  buf2[ len ] = 0;
  unix_to_tos_path( name2, buf2 );
  if( strlen( name2 ) > bufsiz - 1 ) {
    return TOS_ERANGE;
  }
  strcpy( buf, name2 );
  return TOS_E_OK;
}

MINTFUNC(Fcntl)
{
  TOSARG(short,handle);
  TOSARG(long,arg2);
  TOSARG(short,cmd);
  long ret;
  int unix_handle = handle;
  int fd, fp;
  struct stat buf;
  int retcode;

  GETFD( unix_handle );

  switch( cmd ) {
  case TOS_F_DUPFD:
    if( (fp = new_handle()) < 0 ) {
      return fp;
    }
    FdMap[ fp ] = unix_handle;
    if( unix_handle >= 0 ) {
      FdCnt[ unix_handle ]++;
    }
    return fp;

  case TOS_F_GETFD:
    DDEBUG( "  F_GETFD\n" );
    if( (ret = fcntl( unix_handle, F_GETFD )) == -1 ) {
      return translate_error( errno );
    }
    return ret & 1 ? 0 : 1;

  case TOS_F_SETFD:
    DDEBUG( "  G_SETFD\n" );
    if( fcntl( unix_handle, F_SETFD, arg2 ? 0 : 1 ) == -1 ) {
      return translate_error( errno );
    }
    return TOS_E_OK;
  
  case TOS_F_GETFL: printf( "  unimp F_GETFL\n" ); return TOS_EINVFN;
  case TOS_F_SETFL: printf( "  unimp F_SETFL\n" ); return TOS_EINVFN;
  case TOS_F_GETLK: printf( "  unimp F_GETLK\n" ); return TOS_EINVFN;
  case TOS_F_SETLK: printf( "  unimp F_SETLK\n" ); return TOS_EINVFN;
  case TOS_F_SETLKW: printf( "  unimp F_SETLKW\n" ); return TOS_EINVFN;
  case TOS_FSTAT:
    DDEBUG( "  FSTAT\n" );
    if( !IS_NONSTD_HANDLE( handle ) ) {
      return TOS_EIHNDL;
    }
    if( fstat( unix_handle, &buf ) == -1 ) {
      retcode = errno;
      DDEBUG( "  fstat failed: %s\n", strerror( errno ) );
      return translate_error( retcode );
    }
    stat_to_xattr( (Xattr *)arg2, &buf );
    return TOS_E_OK;

  case TOS_FIONREAD: printf( "  unimp FIONREAD\n" ); return TOS_EINVFN;
  case TOS_FIONWRITE: printf( "  unimp FIONWRITE\n" ); return TOS_EINVFN;
  case TOS_TIOCGETP: printf( "  unimp TIOCGETP\n" ); return TOS_EINVFN;
  case TOS_TIOCSETP: printf( "  unimp TIOCSETP\n" ); return TOS_EINVFN;
  case TOS_TIOCGETC: printf( "  unimp TIOCGETC\n" ); return TOS_EINVFN;
  case TOS_TIOCSETC: printf( "  unimp TIOCSETC\n" ); return TOS_EINVFN;
  case TOS_TIOCGLTC: printf( "  unimp TIOCGLTC\n" ); return TOS_EINVFN;
  case TOS_TIOCSLTC: printf( "  unimp TIOCSLTC\n" ); return TOS_EINVFN;

  case TOS_TIOCGPGRP:
    DDEBUG( "  TIOCGPGRP\n" );
    /* This is a bad hack, will change */
    *(long *)arg2 = 0;
    return 0;

  case TOS_TIOCSPGRP: printf( "  unimp TIOCSPGRP\n" ); return TOS_EINVFN;
  case TOS_TIOCGWINSZ: printf( "  unimp TIOCGWINSZ\n" ); return TOS_EINVFN;
  case TOS_TIOCSWINSZ: printf( "  unimp TIOCSWINSZ\n" ); return TOS_EINVFN;
  case TOS_TIOCSTOP: printf( "  unimp TIOCSTOP\n" ); return TOS_EINVFN;
  case TOS_TIOCSTART: printf( "  unimp TIOCSTART\n" ); return TOS_EINVFN;
  case TOS_TIOCGXKEY: printf( "  unimp TIOCGXKEY\n" ); return TOS_EINVFN;
  case TOS_TIOCSXKEY: printf( "  unimp TIOCSXKEY\n" ); return TOS_EINVFN;
  case TOS_PPROCADDR: printf( "  unimp PPROCADDR\n" ); return TOS_EINVFN;
  case TOS_PBASEADDR: printf( "  unimp PBASEADDR\n" ); return TOS_EINVFN;
  case TOS_PCTXTSIZE: printf( "  unimp PCTXTSIZE\n" ); return TOS_EINVFN;
  case TOS_PSETFLAGS: printf( "  unimp PSETFLAGS\n" ); return TOS_EINVFN;
  case TOS_PGETFLAGS: printf( "  unimp PGETFLAGS\n" ); return TOS_EINVFN;
  case TOS_PTRACESFLAGS: printf( "  unimp PTRACESFLAGS\n" ); return TOS_EINVFN;
  case TOS_PTRACEGFLAGS: printf( "  unimp PTRACEGFLAGS\n" ); return TOS_EINVFN;
  case TOS_PTRACEGO: printf( "  unimp PTRACEGO\n" ); return TOS_EINVFN;
  case TOS_PTRACEFLOW: printf( "  unimp PTRACEFLOW\n" ); return TOS_EINVFN;
  case TOS_PTRACESTEP: printf( "  unimp PTRACESTEP\n" ); return TOS_EINVFN;
  case TOS_PTRACE11: printf( "  unimp PTRACE11\n" ); return TOS_EINVFN;
  case TOS_PLOADINFO: printf( "  unimp PLOADINFO\n" ); return TOS_EINVFN;
  case TOS_SHMGETBLK: printf( "  unimp SHMGETBLK\n" ); return TOS_EINVFN;
  case TOS_SHMSETBLK: printf( "  unimp SHMSETBLK\n" ); return TOS_EINVFN;
  default: printf( "  Unknown Fcntl command: 0x%04x\n", cmd );
  }
  return TOS_EBADRQ;
}

MINTFUNC(Dopendir)
{
  TOSARG(char *,path);
  TOSARG(short,flag);
  DopendirDirContext *context;
  static char new_path[ 1024 ];

  TOS_TO_UNIX( new_path, path );
  DDEBUG( "  path after: %s\n", new_path );
  context = mymalloc( sizeof( DopendirDirContext ) );
  context->mode = flag;
  if( (context->dir_pointer = opendir( new_path )) == NULL ) {
    DDEBUG( "  opendir failed: %s\n", strerror( errno ) );
    if( errno == EACCES ) {
      return TOS_EACCDN;
    }
    else {
      return TOS_EPTHNF;
    }
  }
  return (long)context;
}

MINTFUNC(Dreaddir)
{
  TOSARG(short,len);
  TOSARG(DopendirDirContext *,context);
  TOSARG(char *,buf);
  struct dirent *e;
  static char new_fname[ 1024 ];

  if( (e = readdir( context->dir_pointer )) == NULL ) {
    return TOS_ENMFIL;
  }
  DDEBUG( "  file: %s\n", e->d_name );
  if( context->mode != 1 ) {
    if( strlen( e->d_name ) + 5 > len ) {
      return TOS_ERANGE;
    }
    *(long *)buf = e->d_ino;
    strcpy( buf + 4, e->d_name );
  }
  else {
    unix_to_tos_file_short( new_fname, e->d_name );
    if( strlen( new_fname ) + 1 > len ) {
      return TOS_ERANGE;
    }
    strcpy( buf, new_fname );
  }
  return TOS_E_OK;
}
  
MINTFUNC(Drewinddir)
{
  TOSARG(DopendirDirContext *,context);

  rewinddir( context->dir_pointer );
  return TOS_E_OK;
}

MINTFUNC(Dclosedir)
{
  TOSARG(DopendirDirContext *,context);

  if( closedir( context->dir_pointer ) == -1 ) {
    return TOS_EIHNDL;
  }
  free( context );
  return TOS_E_OK;
}

MINTFUNC(Dpathconf)
{
  TOSARG(char *,name);
  TOSARG(int,mode);

  ARG_USED(name);
  
  switch( mode ) {
  case -1:
    return 6;
  case 0:
    return -1;
  case 1:
    return -1;
  case 2:
    return 128;
  case 3:
    return 128;
  case 4:
    return 32;
  case 5:
    return 0;
  case 6:
    return 0;
  default:
    return 0;
  }
}


/*
 *  Converts the contents of a stat buffer ot an Xattr structure
 *
 */

void stat_to_xattr( Xattr *attr, struct stat *buf )
{ 
  if( S_ISCHR( buf->st_mode ) )	attr->mode = TOS_S_IFCHR;
  if( S_ISDIR( buf->st_mode ) )	attr->mode = TOS_S_IFDIR;
  if( S_ISREG( buf->st_mode ) )	attr->mode = TOS_S_IFREG;
  if( S_ISFIFO( buf->st_mode ) ) attr->mode = TOS_S_IFIFO;
  if( S_ISLNK( buf->st_mode ) )	attr->mode = TOS_S_IFLNK;

  if( buf->st_mode & S_IRUSR )	attr->mode |= TOS_S_IRUSR;
  if( buf->st_mode & S_IWUSR )	attr->mode |= TOS_S_IWUSR;
  if( buf->st_mode & S_IXUSR )	attr->mode |= TOS_S_IXUSR;
  if( buf->st_mode & S_IRGRP )	attr->mode |= TOS_S_IRGRP;
  if( buf->st_mode & S_IWGRP )	attr->mode |= TOS_S_IWGRP;
  if( buf->st_mode & S_IXGRP )	attr->mode |= TOS_S_IXGRP;
  if( buf->st_mode & S_IROTH )	attr->mode |= TOS_S_IROTH;
  if( buf->st_mode & S_IWOTH )	attr->mode |= TOS_S_IWOTH;
  if( buf->st_mode & S_IXOTH )	attr->mode |= TOS_S_IXOTH;

  attr->index = buf->st_ino;
  attr->dev = buf->st_dev;
  attr->nlink = buf->st_nlink;
  attr->uid = buf->st_uid;
  attr->gid = buf->st_gid;
  attr->size = buf->st_size;
  attr->blksize = buf->st_blksize;
  attr->nblocks = buf->st_blocks;
  unix_time_to_tos( &attr->mtime, &attr->mdate, buf->st_mtime );
  unix_time_to_tos( &attr->atime, &attr->adate, buf->st_atime );
  unix_time_to_tos( &attr->atime, &attr->cdate, buf->st_ctime );

  if( S_ISDIR( buf->st_mode ) ) {
    attr->attr = TOS_ATTRIB_DIRECTORY;
  }
  else {
    attr->attr = 0;
  }
}


/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
