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

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "bios.h"

extern TosProgram *prog;

TosSystemCall *bios_syscalls[] = {
/*  0 */  bios_Getmpb, bios_Bconstat, bios_Bconin, bios_Bconout, bios_Rwabs,
/*  5 */  bios_Setexc, bios_Tickcal, bios_Getbpb, bios_Bcostat, bios_Mediach, 
/* 10 */  bios_Drvmap, bios_Kbshift,
};

#ifdef DEBUG
char *bios_call_names[] = {
  "Getmpb", "Bconstat", "Bconin", "Bconout", "Rwabs",
  "Setexc", "Tickcal", "Getbpb", "Bcostat", "Mediach", 
  "Drvmap", "Kbshift"
};

char *bios_call_args[] = {
  "", "%hd", "%hd", "%hd, %02hx", "%hd, %08lx, %hd, %hd, %hd, %ld",
  "%03hx, %08lx", "", "%hd", "%hd", "%hd", 
  "", "%02hx"
};

/* Don't use non-32-bit formats for first arg!! */
char *bios_call_retv[] = {
  "%d", "@%d", "%02x", "%d", "%d",
  "#%08lx", "%d", "#%{BPB}", "@%d", "%d", 
  "%08lx", "%02x"
};
#endif

unsigned long dispatch_bios( char *args )
{
  int callnum = *(short *)args;
  long rv;

  if (callnum < 0 || callnum > arraysize(bios_syscalls) ||
      !bios_syscalls[callnum]) {
    DDEBUG( "call to invalid BIOS function #%d\n", callnum );
    return TOS_EINVFN;
  }

  STRACE_BEGIN( bios, args+2 );
  rv = bios_syscalls[ callnum ]( args + 2 );
  STRACE_END( bios, args+2, rv );
  return rv;
}


void init_bios( void )

{
	init_biosdev_fd();
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
