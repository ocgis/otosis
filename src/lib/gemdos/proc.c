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
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "div.h"
#include "prototypes.h"
#include "gemdos.h"
#include "toserrors.h"

extern TosProgram *prog;

GEMDOSFUNC(Pterm0)
{
  DDEBUG("\n");
  rexit( 0 );
}

GEMDOSFUNC(Pterm)
{
  TOSARG(short,code);
  DDEBUG("\n");
  rexit( code );
}

GEMDOSFUNC(Ptermres)
{
  TOSARG(ulong,keep);
  TOSARG(short,code);

  ARG_USED(keep);
  /* This function isn't useful in an emulator,
     so it simply terminates */
  DDEBUG("\n");
  rexit( code );
}

static char **args, **argsp;
static int num_args;

static void new_argsp( void )
{
  int pos;

  if( argsp == NULL ) {
    argsp = args;
  }
  else {
    if( ++argsp - args >= num_args ) {
      pos = argsp - args;
      num_args += 10;
      args = myrealloc( args, sizeof( char * ) * num_args );
      argsp = args + pos;
    }
  }
}

static void new_argsp_malloc( void )
{
  new_argsp();
  *argsp = mymalloc( 1024 );
}


GEMDOSFUNC(Pexec)
{
  TOSARG(short,mode);
  TOSARG(char *,filename);
  TOSARG(char *,cmdline);
  TOSARG(char *,env);
  int pid;
  SIGSTACK_STATIC char new_fname[ 1024 ];
  int env_size;
  char *new_env;
  char *w, *w2;
  char tmp_num[ 5 ];
  int ret;
  int c;
  int status;
  char *argv_p;

  TOS_TO_UNIX( new_fname, filename );

/*
 *  Check for ARGV commandline
 */
  argv_p = NULL;
  if( *cmdline == 127 ) {
    /* called using ARGV */
    for( w = env ; w[ 0 ] || w[ 1 ] ; w++ ) {
      if( memcmp( w, "\000ARGV=", 6 ) == 0 ) {
	DDEBUG( "  found ARGV\n" );
	w[ 1 ] = 0;
	w += 6;

	/* find argv[0] */
	while( *w )  w++;
	w++;
	DDEBUG( "  command name: %s\n", w );

	/* find arguments */
	while( *w )  w++;
	w++;
	argv_p = w;
	DDEBUG( "  argvp: %s\n", argv_p );
	break;
      }
    }
  }


  args = mymalloc( sizeof( char * ) * 10 );
  argsp = NULL;
  num_args = 10;

  new_argsp();
  *argsp = mystrdup( "tos" );
  /* propagate debug and trace flags */
  if( Opt_debug ) {
    new_argsp();
    *argsp = mystrdup( "--debug" );
    DDEBUG( "  setting option: %s\n", *argsp );
  }
  if( Opt_trace ) {
    new_argsp();
    *argsp = mystrdup( "--trace" );
    DDEBUG( "  setting option: %s\n", *argsp );
  }
  for( c = 0 ; c <= 'z' - 'a' ; c++ ) {
    if( IS_DRV( c ) ) {
      new_argsp_malloc();
      sprintf( *argsp, "--drive%c=%s", c + 'A', prog->drive_map[ c ] );
      DDEBUG( "  setting option: %s\n", *argsp );
      new_argsp_malloc();
      sprintf( *argsp, "--current%c=%s", c + 'A', prog->curdir[ c ] );
      DDEBUG( "  setting option: %s\n", *argsp );
    }
  }
  new_argsp_malloc();
  sprintf( *argsp, "--current_drive=%c", prog->curdrv + 'a' );
  DDEBUG( "  setting option: %s\n", *argsp );

#define SET_ENV_OPT "--environment="
  if( env == NULL ) {
    new_env = SET_ENV_OPT "-";
  }
  else {
    for( env_size = 0 ; env[ env_size ] || env[ env_size + 1 ] ; env_size++ );
    /* Worst case size */
    new_env = mymalloc( (env_size + 1) * 4 + strlen( SET_ENV_OPT ) );
    strcpy( new_env, SET_ENV_OPT );
    for( w = env, w2 = new_env + strlen( SET_ENV_OPT ) ;
	w[ 0 ] || w[ 1 ] ; w++, w2++ ) {
      if( isalnum( *w ) ) {
	*w2 = *w;
      }
      else {
	*w2++ = '\\';
	sprintf( tmp_num, "%03o", *w );
	*w2++ = tmp_num[ 0 ];
	*w2++ = tmp_num[ 1 ];
	*w2 = tmp_num[ 2 ];
      }
    }
    *w2 = 0;
  }
  new_argsp();
  *argsp = new_env;
  DDEBUG( "  setting option: %s\n", *argsp );
  new_argsp();
  *argsp = mystrdup( "--" );
  DDEBUG( "  setting option: %s\n", *argsp );
  new_argsp();
  *argsp = mystrdup( new_fname );
  DDEBUG( "  setting option: %s\n", *argsp );
	

  /*
   *  Set up commandline
   */
  if( argv_p ) {
    while( *argv_p ) {
      new_argsp();
      *argsp = mystrdup( argv_p );
      DDEBUG( "  arg: %s\n", *argsp );
      argv_p += strlen( argv_p ) + 1;
    }
  }
  else {
    new_argsp();
    *argsp = mystrdup( cmdline + 1 );
    DDEBUG( "  commandline: %s\n", *argsp );
  }
  new_argsp();
  *argsp = NULL;
    


  switch( mode ) {
  case TOS_PEXEC_LOAD_GO:
    if( (pid = fork()) == -1 ) {
      /* fork failed */
      DDEBUG( "  fork failed\n" );
      ret = TOS_ERROR;
    }
    else if( pid == 0 ) {
	  /* Hmm... should be proceed out current command line to the child??
	   * Currently left out for easiness of implementation :-) */
	  /* BUG: splitting of 'cmdline' missing! */
      execv( "/proc/self/exe", args );
      exit( translate_error( errno ) );
    }
    else {
      waitpid( pid, &status, 0 );
      ret = status;
    }
    break;
  case TOS_PEXEC_LOAD_DONTDO:
    DDEBUG( "  TOS_PEXEC_LOAD_DONTDO\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_GO:
    DDEBUG( "  TOS_PEXEC_GO\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_CREATE_BASEPAGE:
    DDEBUG( "  TOS_PEXEC_CREATE_BASEPAGE\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_GO_AND_FREE:
    DDEBUG( "  TOS_PEXEC_GO_AND_FREE\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_LOAD_GO_NOWAIT:
    DDEBUG( "  TOS_PEXEC_LOAD_GO_NOWAIT\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_GO_NOWAIT:
    DDEBUG( "  TOS_PEXEC_GO_NOWAIT\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_GO_NOWAIT_NOSHARING:
    DDEBUG( "  TOS_PEXEC_GO_NOWAIT_NOSHARING\n" );
    ret = TOS_EINVFN;
    break;
  case TOS_PEXEC_REPLACE_PROGRAM_GO:
    DDEBUG( "  TOS_PEXEC_REPLACE_PROGRAM_GO\n" );
    ret = TOS_EINVFN;
    break;
  default:
    printf( "Unknown Pexec mode: %d\n", mode );
    ret = TOS_EBADRQ;
  }

  for( argsp = args ; *argsp ; argsp++ ) {
    free( *argsp );
  }

  return ret;
}

GEMDOSFUNC(Super)
{
  TOSARG(ulong,arg);
  
  if (arg == 1)
	  return prog->super ? -1 : 0;
  else if (arg == 0) {
	  prog->super = 1;
	  return 2;
  }
  else if (arg > 1) {
	  prog->super = !prog->super;
	  return TOS_E_OK;
  }
  else
	  return TOS_EACCDN;
}

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
