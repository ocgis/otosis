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
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#include <div.h>
#include <prototypes.h>
#include <toserrors.h>
#include <mint.h>

extern TosProgram *prog;

MINTFUNC(Syield)
{
  /*
   *  This function does nothing
   */
  return TOS_E_OK;
}

MINT_UNIMP(Fpipe);
MINT_UNIMP(Fgetchar);
MINT_UNIMP(Fputchar);
MINT_UNIMP(Pwait);

MINTFUNC(Pnice)
{
  /*
   *  Changing of process priorities is not supported
   *  via this emulator
   */
  return 0;
}

MINTFUNC(Pgetpid)
{
  return getpid();
}

MINTFUNC(Pgetppid)
{
  return getppid();
}

static int pgrp = 0;
MINTFUNC(Pgetpgrp)
{
  /*
   *  This isn't useful until we have a way of emulating signals
   */
  return pgrp;
}

MINTFUNC(Psetpgrp)
{
  TOSARG(short,pid);
  TOSARG(short,newgrp);

  pgrp = newgrp;
  return pgrp;
}

MINTFUNC(Pgetuid)
{
  return getuid();
}

MINTFUNC(Psetuid)
{
  TOSARG(short,uid);

  if( setuid( uid ) == -1 ) {
    return translate_error( errno );
  }
  return uid;
}

MINTFUNC(Pkill)
{
  TOSARG(short,pid);
  TOSARG(short,sig);

  if( kill( pid, sig ) == -1 ) {
    return translate_error( errno );
  }
  return 0;
}

MINT_UNIMP(Psignal);

MINTFUNC(Pvfork)
{
  return mint_Pfork( _args );
}

MINTFUNC(Pgetgid)
{
  return getgid();
}

MINTFUNC(Psetgid)
{
  TOSARG(short,gid);

  if( setgid( gid ) == -1 ) {
    return translate_error( errno );
  }
  return gid;
}

/*
 *  Same thing applies here as to the Psetpgrp and Pgetpgrp
 *  calls. Signal emulation needs to be implemented first.
 */
MINTFUNC(Psigblock)
{
  return 0;
}

MINT_UNIMP(Psigsetmask);

MINTFUNC(Pusrval)
{
  TOSARG(long,val);
  long oldval = prog->usrval;

  prog->usrval = val;
  return oldval;
}

MINTFUNC(Pdomain)
{
  TOSARG(short,newdomain);
  int olddomain = prog->domain;

  if( newdomain == 0 || newdomain == 1 ) {
    prog->domain = newdomain;
  }
  return olddomain;
}

MINT_UNIMP(Psigreturn);

MINTFUNC(Pfork)
{
  int pid;

  if( (pid = fork()) == -1 ) {
    return translate_error( errno );
  }
  return pid;
}

MINT_UNIMP(Pwait3);
MINT_UNIMP(Fselect);
MINT_UNIMP(Prusage);
MINT_UNIMP(Psetlimit);
MINT_UNIMP(Talarm);
MINT_UNIMP(Pause);

MINTFUNC(Sysconf)
{
  TOSARG(short,val);

  /*
   *  The return values of these are not really correct
   */
  switch( val ) {
  case -1:			/* Maximum value for val */
    return 4;
  case 0:			/* Max memory regions per process */
    return -1;
  case 1:			/* Max length of command string */
    return -1;
  case 2:			/* Max open files per process */
    return -1;
  case 3:			/* Max supplementary group ID's */
    return -1;
  case 4:			/* Max processes per user */
    return -1;
  }
  return TOS_ERROR;
}

MINT_UNIMP(Psigpending);
MINT_UNIMP(Pmsg);
MINT_UNIMP(Fmidipipe);

MINTFUNC(Prenice)
{
  /*
   *  Process priorites are not emulated
   */
  return 0;
}

MINT_UNIMP(Flink);
MINT_UNIMP(Fsymlink);
MINT_UNIMP(Dcntl);

MINTFUNC(Pumask)
{
  TOSARG(short,mode);

  return umask( mode );
}

MINT_UNIMP(Psemaphore);
MINT_UNIMP(Dlock);
MINT_UNIMP(Psigpause);
MINT_UNIMP(Psigaction);

MINTFUNC(Pgeteuid)
{
  return geteuid();
}

MINTFUNC(Pgetegid)
{
  return getegid();
}

MINT_UNIMP(Pwaitpid);
MINT_UNIMP(Salert);

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
