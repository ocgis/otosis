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

#ifndef MINT_CALLS_H
#define MINT_CALLS_H

MINTFUNC(Syield);
MINTFUNC(Fpipe);
MINTFUNC(Fcntl);
MINTFUNC(Finstat);
MINTFUNC(Foutstat);
MINTFUNC(Fgetchar);
MINTFUNC(Fputchar);
MINTFUNC(Pwait);
MINTFUNC(Pnice);
MINTFUNC(Pgetpid);
MINTFUNC(Pgetppid);
MINTFUNC(Pgetpgrp);
MINTFUNC(Psetpgrp);
MINTFUNC(Pgetuid);
MINTFUNC(Psetuid);
MINTFUNC(Pkill);
MINTFUNC(Psignal);
MINTFUNC(Pvfork);
MINTFUNC(Pgetgid);
MINTFUNC(Psetgid);
MINTFUNC(Psigblock);
MINTFUNC(Psigsetmask);
MINTFUNC(Pusrval);
MINTFUNC(Pdomain);
MINTFUNC(Psigreturn);
MINTFUNC(Pfork);
MINTFUNC(Pwait3);
MINTFUNC(Fselect);
MINTFUNC(Prusage);
MINTFUNC(Psetlimit);
MINTFUNC(Talarm);
MINTFUNC(Pause);
MINTFUNC(Sysconf);
MINTFUNC(Psigpending);
MINTFUNC(Dpathconf);
MINTFUNC(Pmsg);
MINTFUNC(Fmidipipe);
MINTFUNC(Prenice);
MINTFUNC(Dopendir);
MINTFUNC(Dreaddir);
MINTFUNC(Drewinddir);
MINTFUNC(Dclosedir);
MINTFUNC(Fxattr);
MINTFUNC(Flink);
MINTFUNC(Fsymlink);
MINTFUNC(Freadlink);
MINTFUNC(Dcntl);
MINTFUNC(Fchown);
MINTFUNC(Fchmod);
MINTFUNC(Pumask);
MINTFUNC(Psemaphore);
MINTFUNC(Dlock);
MINTFUNC(Psigpause);
MINTFUNC(Psigaction);
MINTFUNC(Pgeteuid);
MINTFUNC(Pgetegid);
MINTFUNC(Pwaitpid);
MINTFUNC(Dgetcwd);
MINTFUNC(Salert);

unsigned long dispatch_mint( char * );

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
