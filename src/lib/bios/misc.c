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
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "div.h"
#include "prototypes.h"
#include "bios.h"
#include "toserrors.h"


extern TosProgram *prog;


/* Shouldn't be called by user programs anyway... */
BIOS_UNIMP(Getmpb);

/* Not emulatable... except maybe for floppy disks */
BIOS_UNIMP(Rwabs);

/* Should we look for setting etv_timer, etv_crit, etv_term? These could be
 * useful... */
BIOS_UNIMP(Setexc);

BIOSFUNC(Tickcal)
{
  return 5;
}

BIOSFUNC(Getbpb)
{
  TOSARG(short,dev);
  static Bpb bpbdata;

  /* There is no way this function can be emulated properly,
     so this function just returns some arbitary values */
	/* ++roman: Could make sense for floppy devices... */

  ARG_USED(dev);

  bpbdata.recsiz = 512;
  bpbdata.clsiz = 2;
  bpbdata.clsizb = 1024;
  bpbdata.rdlen = 16;
  bpbdata.fsiz = 16;
  bpbdata.fatrec = 32;
  bpbdata.datrec = 64;
  bpbdata.numcl = 200;
  bpbdata.bflags = 1;

  return (long)&bpbdata;
}

BIOSFUNC(Mediach)
{
  TOSARG(short,dev);

  CHECK_DRV( dev );
  return 0;
}

BIOSFUNC(Drvmap)
{
  int map = 0;
  int c;

  for( c = 0 ; c <= 'z' - 'a' ; c++ ) {
    if( IS_DRV( c ) ) {
      map |= (1 << c);
    }
  }
  return map;
}

BIOS_UNIMP(Kbshift);


/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
