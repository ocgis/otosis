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

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "mint.h"
#include "init.h"

#define	UNUSED	(TosSystemCall *)NULL

extern TosProgram *prog;

TosSystemCall *mint_syscalls[] = {
  /* 255 */  mint_Syield, mint_Fpipe, UNUSED, UNUSED, UNUSED,
  /* 260 */  mint_Fcntl, mint_Finstat, mint_Foutstat, mint_Fgetchar, mint_Fputchar,
  /* 265 */  mint_Pwait, mint_Pnice, mint_Pgetpid, mint_Pgetppid, mint_Pgetpgrp,
  /* 270 */  mint_Psetpgrp, mint_Pgetuid, mint_Psetuid, mint_Pkill, mint_Psignal,
  /* 275 */  mint_Pvfork, mint_Pgetgid, mint_Psetgid, mint_Psigblock, mint_Psigsetmask,
  /* 280 */  mint_Pusrval, mint_Pdomain, mint_Psigreturn, mint_Pfork, mint_Pwait3,
  /* 285 */  mint_Fselect, mint_Prusage, mint_Psetlimit, mint_Talarm, mint_Pause,
  /* 290 */  mint_Sysconf, mint_Psigpending, mint_Dpathconf, mint_Pmsg, mint_Fmidipipe,
  /* 295 */  mint_Prenice, mint_Dopendir, mint_Dreaddir, mint_Drewinddir, mint_Dclosedir,
  /* 300 */  mint_Fxattr, mint_Flink, mint_Fsymlink, mint_Freadlink, mint_Dcntl,
  /* 305 */  mint_Fchown, mint_Fchmod, mint_Pumask, mint_Psemaphore, mint_Dlock,
  /* 310 */  mint_Psigpause, mint_Psigaction, mint_Pgeteuid, mint_Pgetegid, mint_Pwaitpid,
  /* 315 */  mint_Dgetcwd, mint_Salert,
};

#ifdef DEBUG
char *mint_call_names[] = {
  "Syield", "Fpipe", "unused", "unused", "unused", "Fcntl",
  "Finstat", "Foutstat", "Fgetchar", "Fputchar", "Pwait", "Pnice",
  "Pgetpid", "Pgetppid", "Pgetpgrp", "Psetpgrp", "Pgetuid", "Psetuid",
  "Pkill", "Psignal", "Pvfork", "Pgetgid", "Psetgid", "Psigblock",
  "Psigsetmask", "Pusrval", "Pdomain", "Psigreturn", "Pfork", "Pwait3",
  "Fselect", "Prusage", "Psetlimit", "Talarm", "Pause", "Sysconf",
  "Psigpending", "Dpathconf", "Pmsg", "Fmidipipe", "Prenice", "Dopendir",
  "Dreaddir", "Drewinddir", "Dclosedir", "Fxattr", "Flink", "Fsymlink",
  "Freadlink", "Dcntl", "Fchown", "Fchmod", "Pumask", "Psemaphore",
  "Dlock", "Psigpause", "Psigaction", "Pgeteuid", "Pgetegid", "Pwaitpid",
  "Dgetcwd", "Salert",
};

/* Some formats still need to be filled in */
char *mint_call_args[] = {
  "", "Fpipe", NULL, NULL, NULL, "%hd, %08lx, %04hx",
  "%hd", "%hd", "Fgetchar", "Fputchar", "Pwait", "Pnice",
  "", "", "", "Psetpgrp", "", "%hd",
  "%hd, %hd", "Psignal", "", "", "%hd", "Psigblock",
  "Psigsetmask", "%08lx", "%hd", "Psigreturn", "", "Pwait3",
  "Fselect", "Prusage", "Psetlimit", "Talarm", "Pause", "%hd",
  "Psigpending", "%s, %ld", "Pmsg", "Fmidipipe", "Prenice", "%s, %hd",
  "%hd, %08lx, %s", "%08lx", "%08lx", "%hd, %s, %{XATTR}", "Flink", "Fsymlink",
  "%hd, %s, %08lx", "Dcntl", "%s, %hd, %hd", "%s, 0%03ho", "0%03ho", "Psemaphore",
  "Dlock", "Psigpause", "Psigaction", "", "", "Pwaitpid",
  "%s, %hd, %hd", "Salert",
};

/* Don't use non-32-bit formats for first arg!! */
char *mint_call_retv[] = {
  "%d", "Fpipe", NULL, NULL, NULL, "%d",
  "@%d", "@%d", "Fgetchar", "Fputchar", "Pwait", "%d",
  "%d", "%d", "%d", "%s", "%d", "%d",
  "%d", "Psignal", "%d", "%d", "%d", "Psigblock",
  "Psigsetmask", "#%08lx", "%d", "Psigreturn", "%d", "Pwait3",
  "Fselect", "Prusage", "Psetlimit", "Talarm", "Pause", "@%d",
  "Psigpending", "#%d", "Pmsg", "Fmidipipe", "Prenice", "%08lx",
/*  "%d %hS%S%s", "%d", "%d", "%d %hS%S%{XATTR}", "Flink", "Fsymlink",*/
  "%d %hS%S%s", "%d", "%d", "%d", "Flink", "Fsymlink",
  "%d, %hS%s", "Dcntl", "%d", "%d", "%03o", "Psemaphore",
  "Dlock", "Psigpause", "Psigaction", "%d", "%d", "Pwaitpid",
  "%d %s", "Salert",
};
#endif

unsigned long dispatch_mint(char * _args)
{
  TOSARG(short, callnum);
  long rv;

  /* Allocate a program structure if needed */
  if (prog == NULL) {
    prog = new_program ();
  }

  callnum -= 255;
  if (callnum < 0 || callnum > arraysize(mint_syscalls) ||
      !mint_syscalls[callnum]) {
    DDEBUG( "call to invalid MiNT function #%d\n", callnum+255 );
    return TOS_EINVFN;
  }

  STRACE_BEGIN(mint, _args);
  rv = mint_syscalls[callnum](_args);
  STRACE_END(mint, _args, rv);
  return rv;
}

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
