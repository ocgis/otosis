/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
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

#include <stdio.h>
#include <unistd.h>

#include <vdibind.h>

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "xgemdos.h"

extern TosProgram *prog;

#define	UNUSED	(TosSystemCall *)NULL

TosSystemCall *xgemdos_syscalls[] = {
/*   0 */  xgemdos_Warmstart, UNUSED, UNUSED, UNUSED, UNUSED,
/*   5 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  10 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  15 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  20 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  25 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  30 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  35 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  40 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  45 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  50 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  55 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  60 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  65 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  70 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  75 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  80 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  85 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  90 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  95 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 100 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 105 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 110 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 115 */  xgemdos_VDI, UNUSED, UNUSED, UNUSED, UNUSED,
/* 120 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 125 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 130 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 135 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 140 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 145 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 150 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 155 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 160 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 165 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 170 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 175 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 180 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 185 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 190 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 195 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/* 200 */  xgemdos_AES
};

#ifdef DEBUG
char *xgemdos_call_names[] = {
  "Warmstart", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "VDI", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused", "unused",
  "AES"
};

char *xgemdos_call_args[] = {
  "", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, "%08lx", NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  "%08lx"
};
#endif

/* arguments are sent through d0 and d1 instead of the stack for xgemdos */
unsigned long dispatch_xgemdos( short d0, unsigned long d1 )
{
  int callnum = d0;

  if (callnum < 0 || callnum > arraysize(xgemdos_syscalls) ||
      !xgemdos_syscalls[callnum]) {
    DDEBUG( "call to invalid XGEMDOS function #%d\n", callnum );
    return TOS_EINVFN;
  }

  STRACE_BEGIN( xgemdos, (char *)&d1 );
  xgemdos_syscalls[ callnum ]( (char *)&d1 );

  return 0;
}


/* ---------------------------------------------------------------------- */

static int vdi_handle;

void init_xgemdos( void )
{
  int work_in[11] = { 1,1,1,1,1,1,1,1,1,1,2 };
  int work_out[57];
    
  v_opnwk(work_in, &vdi_handle, work_out);
}

void exit_xgemdos( void )
{
  v_clswk(vdi_handle);
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
