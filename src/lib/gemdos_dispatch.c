/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *  Copyright 1999 Christer Gustavsson <cg@nocrew.org>
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

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "gemdos.h"
#include "init.h"
#include "mint.h"

#include "fd.h"

extern TosProgram *prog;

#define	UNUSED	(TosSystemCall *)NULL

TosSystemCall *gemdos_syscalls[] = {
/*  0 */  gemdos_Pterm0, gemdos_Cconin, gemdos_Cconout, gemdos_Cauxin, gemdos_Cauxout,
/*  5 */  gemdos_Cprnout, gemdos_Crawio, gemdos_Crawcin, gemdos_Cnecin, gemdos_Cconws,
/* 10 */  gemdos_Cconrs, gemdos_Cconis, UNUSED, UNUSED, gemdos_Dsetdrv,
/* 15 */  UNUSED, gemdos_Cconos, gemdos_Cprnos, gemdos_Cauxis, gemdos_Cauxos,
/* 20 */  gemdos_Maddalt, UNUSED, UNUSED, UNUSED, UNUSED,
/* 25 */  gemdos_Dgetdrv, gemdos_Fsetdta, UNUSED, UNUSED, UNUSED,
/* 30 */  UNUSED, UNUSED, gemdos_Super, UNUSED, UNUSED, 
/* 35 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 40 */  UNUSED, UNUSED, gemdos_Tgetdate, gemdos_Tsetdate, gemdos_Tgettime,
/* 45 */  gemdos_Tsettime, UNUSED, gemdos_Fgetdta, gemdos_Sversion, gemdos_Ptermres,
/* 50 */  UNUSED, UNUSED, UNUSED, UNUSED, gemdos_Dfree,
/* 55 */  UNUSED, UNUSED, gemdos_Dcreate, gemdos_Ddelete, gemdos_Dsetpath,
/* 60 */  gemdos_Fcreate, gemdos_Fopen, gemdos_Fclose, gemdos_Fread, gemdos_Fwrite,
/* 65 */  gemdos_Fdelete, gemdos_Fseek, gemdos_Fattrib, gemdos_Mxalloc, gemdos_Fdup,
/* 70 */  gemdos_Fforce, gemdos_Dgetpath, gemdos_Malloc, gemdos_Mfree, gemdos_Mshrink,
/* 75 */  gemdos_Pexec, gemdos_Pterm, UNUSED, gemdos_Fsfirst, gemdos_Fsnext,
/* 80 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 85 */  UNUSED, gemdos_Frename, gemdos_Fdatime, UNUSED, UNUSED,
/* 90 */  UNUSED, UNUSED, gemdos_Flock,
};

#ifdef DEBUG
char *gemdos_call_names[] = {
  "Pterm0", "Cconin", "Cconout", "Cauxin", "Cauxout", "Cprnout",
  "Crawio", "Crawcin", "Cnecin", "Cconws", "Cconrs", "Cconis",
  "unused", "unused", "Dsetdrv", "unused", "Cconos", "Cprnos",
  "Cauxis", "Cauxos", "Maddalt", "unused", "unused", "unused",
  "unused", "Dgetdrv", "Fsetdta", "unused", "unused", "unused",
  "unused", "unused", "Super", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "Tgetdate", "Tsetdate", "Tgettime", "Tsettime", "unused", "Fgetdta",
  "Sversion", "Ptermres", "unused", "unused", "unused", "unused",
  "Dfree", "unused", "unused", "Dcreate", "Ddelete", "Dsetpath",
  "Fcreate", "Fopen", "Fclose", "Fread", "Fwrite", "Fdelete",
  "Fseek", "Fattrib", "Mxalloc", "Fdup", "Fforce", "Dgetpath",
  "Malloc", "Mfree", "Mshrink", "Pexec", "Pterm", "unused",
  "Fsfirst", "Fsnext", "unused", "unused", "unused", "unused",
  "unused", "unused", "Frename", "Fdatime", "unused", "unused",
  "unused", "unused", "Flock"
};

char *gemdos_call_args[] = {
  "", "", "%02hx", "", "%02hx", "%02hx",
  "%02hx", "", "", "%s", "%{LINE}", "",
  NULL, NULL, "%04hd", NULL, "", "",
  "", "", "%08lx, %ld", NULL, NULL, NULL,
  NULL, "", "%08lx", NULL, NULL, NULL,
  NULL, NULL, "%08lx", NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL,
  "", "%D", "", "%T", NULL, "",
  "", "%ld, %hd", NULL, NULL, NULL, NULL,
  "%08lx, %hd", NULL, NULL, "%s", "%s", "%s",
  "%s, %02hx", "%s, %02hx", "%hd", "%hd, %ld, %08lx", "%hd, %ld, %08lx", "%s",
  "%ld, %hd, %hd", "%s, %hd, %02hx", "%ld, %hd", "%hd", "%hd, %hd", "%S%hd",
  "%ld", "%08lx", "%hd, %08lx, %ld", "%hd, %s, %s, %08lx", "%hd", NULL,
  "%s, %02hx", "", NULL, NULL, NULL, NULL,
  NULL, NULL, "%hd, %s, %s", "%{DOSTIME}, %hd, %hd", NULL, NULL,
  NULL, NULL, "%hd, %hd, %ld, %ld"
};

/* Don't use non-32-bit formats for first arg!! (except %D and %T) */
char *gemdos_call_retv[] = {
  "", "%02x", "%d", "%02x", "%d", "%d",
  "%02x", "%02x", "%02x", "%d", "%d %{LINE}", "@%d",
  NULL, NULL, "#%08lx", NULL, "@%d", "@%d",
  "@%d", "@%d", "%d", NULL, NULL, NULL,
  NULL, "%d", "%d", NULL, NULL, NULL,
  NULL, NULL, "#%08lx", NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL,
  "%D", "%d", "%T", "%d", NULL, "#%{DTA}",
  "%04x", "", NULL, NULL, NULL, NULL,
  "%d %{DISKINFO}", NULL, NULL, "%d", "%d", "%d",
  "%d", "%d", "%d", "%ld", "%ld", "%d",
  "%ld", "%02x", "#%08lx", "%d", "%d", "%d %s",
  "#%08lx", "%d", "%d", "%lx", "", NULL,
  "%d", "%d", NULL, NULL, NULL, NULL,
  NULL, NULL, "%d", "%d %{DOSTIME}", NULL, NULL,
  NULL, NULL, "%d"
};
#endif

unsigned long dispatch_gemdos(char * _args)
{
  TOSARG(short, callnum);
  long rv;

  /* Allocate a program structure if needed */
  if (prog == NULL) {
    prog = new_program ();
  }

  /* Check for possible MiNT call */
  if (prog->emulate_mint && callnum >= 255) {
    return dispatch_mint(_args - 2);
  }

  if (callnum < 0 || callnum > arraysize(gemdos_syscalls) ||
      !gemdos_syscalls[callnum]) {
    DDEBUG( "call to invalid GEMDOS function #%d\n", callnum );
    return TOS_EINVFN;
  }

  STRACE_BEGIN(gemdos, _args);
  rv = gemdos_syscalls[ callnum ](_args);
  STRACE_END( gemdos, _args, rv );
  return rv;
}


/* ---------------------------------------------------------------------- */

/* global variables of GEMDOS module */

DirContext find_info;

int FdMap[MAX_GEMDOS_FD];
int FdCnt[MAX_UNIX_FD];

void init_gemdos( void )

{	int i;
	
	/* initialize file handle mapping */
	FdMap[GEMDOS_STDIN]  = GEMDOS_DEV_CON;
	FdMap[GEMDOS_STDOUT] = GEMDOS_DEV_CON;
	FdMap[GEMDOS_STDAUX] = Opt_aux_stderr ? GEMDOS_DEV_CON : GEMDOS_DEV_AUX;
	FdMap[GEMDOS_STDPRN] = GEMDOS_DEV_PRN;
	FdMap[GEMDOS_STD4]   = GEMDOS_DEV_CON;
	FdMap[GEMDOS_STD5]   = GEMDOS_DEV_CON;
	for( i = GEMDOS_STD5+1; i < MAX_GEMDOS_FD; ++i )
		FdMap[i] = HANDLE_FREE;
	/* all Unix fd's are unused */
	for( i = MIN_UNIX_FD; i < MAX_UNIX_FD; ++i )
		FdCnt[i] = 0;
	
	/* emulate a GEMDOS redirection if Unix stdin or stdout isn't a tty */
	if (!isatty(0))
		FdMap[GEMDOS_STDIN] = 0;
	if (!isatty(1))
		FdMap[GEMDOS_STDOUT] = 1;
	
	find_info.dir_pointer = NULL;
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
