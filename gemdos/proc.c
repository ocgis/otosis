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

#include <div.h>
#include <prototypes.h>
#include <gemdos.h>
#include <toserrors.h>

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

GEMDOSFUNC(Pexec)
{
  TOSARG(short,mode);
  TOSARG(char *,filename);
  TOSARG(char *,cmdline);
  TOSARG(char *,env);
  int pid;
  SIGSTACK_STATIC char new_fname[ 1024 ];

  ARG_USED(env);
  
  switch( mode ) {
  case TOS_PEXEC_LOAD_GO:
    if( (pid = fork()) == -1 ) {
      /* fork failed */
      DDEBUG( "  fork failed\n" );
      return TOS_ERROR;
    }
    else if( pid == 0 ) {
      tos_to_unix( new_fname, filename );
	  /* Hmm... should be proceed out current command line to the child??
	   * Currently left out for easiness of implementation :-) */
	  /* BUG: splitting of 'cmdline' missing! */
      execl( "/proc/self/exe", "tos", new_fname, cmdline, (char *)NULL );
      return translate_error( errno );
    }
    else {
      waitpid( pid, NULL, 0 );
    }
    return TOS_E_OK;
    break;
  case TOS_PEXEC_LOAD_DONTDO:
    DDEBUG( "  TOS_PEXEC_LOAD_DONTDO\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_GO:
    DDEBUG( "  TOS_PEXEC_GO\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_CREATE_BASEPAGE:
    DDEBUG( "  TOS_PEXEC_CREATE_BASEPAGE\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_GO_AND_FREE:
    DDEBUG( "  TOS_PEXEC_GO_AND_FREE\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_LOAD_GO_NOWAIT:
    DDEBUG( "  TOS_PEXEC_LOAD_GO_NOWAIT\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_GO_NOWAIT:
    DDEBUG( "  TOS_PEXEC_GO_NOWAIT\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_GO_NOWAIT_NOSHARING:
    DDEBUG( "  TOS_PEXEC_GO_NOWAIT_NOSHARING\n" );
    return TOS_EINVFN;
    break;
  case TOS_PEXEC_REPLACE_PROGRAM_GO:
    DDEBUG( "  TOS_PEXEC_REPLACE_PROGRAM_GO\n" );
    return TOS_EINVFN;
    break;
  default:
    printf( "Unknown Pexec mode: %d\n", mode );
    return TOS_EBADRQ;
  }
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
