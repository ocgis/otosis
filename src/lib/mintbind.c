/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1998 Tomas Berndtsson <tomas@nocrew.org>
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

#include "div.h"
#include "mint.h"
#include "bindproto.h"

typedef void voidfuncl(long);

bindproto1(mint,long,Dclosedir,0x12b,long);
bindproto3(mint,long,Dcntl,0x130,short,char *,long);
bindproto3(mint,long,Dgetcwd,0x13b,char *,short,short);
bindproto2(mint,long,Dlock,0x135,short,short);
bindproto2(mint,long,Dopendir,0x128,char *,short);
bindproto2(mint,long,Dpathconf,0x124,char *,short);
bindproto3(mint,long,Dreaddir,0x129,short,long,char *);
bindproto1(mint,long,Drewinddir,0x12a,long);
bindproto2(mint,long,Fchmod,0x132,char *,short);
bindproto3(mint,long,Fchown,0x131,char *,short,short);
bindproto3(mint,long,Fcntl,0x104,short,long,short);
bindproto2(mint,long,Fgetchar,0x107,short,short);
bindproto1(mint,long,Finstat,0x105,short);
bindproto2(mint,long,Flink,0x12d,char *,char *);
bindproto3(mint,long,Fmidipipe,0x126,short,short,short);
bindproto1(mint,long,Foutstat,0x106,short);
bindproto1(mint,long,Fpipe,0x100,short *);
bindproto3(mint,long,Fputchar,0x108,short,long,short);
bindproto3(mint,long,Freadlink,0x12f,short,char *,char *);
bindproto4(mint,short,Fselect,0x11d,short,long *,long *,long);
bindproto2(mint,long,Fsymlink,0x12e,char *,char *);
bindproto3(mint,long,Fxattr,0x12c,short,char *,XATTR *);
bindproto0v(mint,Pause,0x121);
bindproto1(mint,short,Pdomain,0x119,short);
bindproto0(mint,short,Pfork,0x11b);
bindproto0(mint,short,Pgetegid,0x139);
bindproto0(mint,short,Pgeteuid,0x138);
bindproto0(mint,short,Pgetgid,0x10f);
bindproto0(mint,short,Pgetpgrp,0x10d);
bindproto0(mint,short,Pgetpid,0x10b);
bindproto0(mint,short,Pgetppid,0x10c);
bindproto0(mint,short,Pgetuid,0x10f);
bindproto2(mint,short,Pkill,0x111,short,short);
bindproto3(mint,short,Pmsg,0x125,short,long,PMSG *);
bindproto1(mint,short,Pnice,0x10a,short);
bindproto2(mint,long,Prenice,0x127,short,short);
bindproto1v(mint,Prusage,0x11e,long *);
bindproto3(mint,long,Psemaphore,0x134,short,long,long);
bindproto1(mint,short,Psetgid,0x115,short);
bindproto2(mint,long,Psetlimit,0x11f,short,long);
bindproto2(mint,long,Psetpgrp,0x10e,short,short);
bindproto1(mint,short,Psetuid,0x110,short);
bindproto3(mint,long,Psigaction,0x137,short,SIGACTION *,SIGACTION *);
bindproto1(mint,long,Psigblock,0x116,long);
bindproto2(mint,long,Psignal,0x112,short,voidfuncl *);
bindproto1(mint,long,Psigpause,0x136,long);
bindproto0(mint,long,Psigpending,0x123);
bindproto0v(mint,Psigreturn,0x11a);
bindproto1(mint,long,Psigsetmask,0x117,long);
bindproto1(mint,short,Pumask,0x133,short);
bindproto1(mint,long,Pusrval,0x118,long);
bindproto0(mint,short,Pvfork,0x113);
bindproto0(mint,long,Pwait,0x109);
bindproto2(mint,long,Pwait3,0x11c,short,long *);
bindproto3(mint,long,Pwaitpid,0x13a,short,short,long *);
bindproto1v(mint,Salert,0x13c,char *);
bindproto0v(mint,Syield,0xff);
bindproto1(mint,long,Sysconf,0x122,short);
bindproto1(mint,long,Talarm,0x120,long);
