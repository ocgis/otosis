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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <asm/ptrace.h>
#include <asm/traps.h>

#include <traps.h>
#include <div.h>
#include <prototypes.h>


extern int in_emu;


void sigsegv_handler( int sig, int vecnum, struct sigcontext *scp )
{
  unsigned long ea, pc, ssw, value;
  int read, size;
  int format = (scp->sc_formatvec >> 12) & 0xf;
  Frame *framedata = (Frame *)(scp + 1);
  int code2size_030[4] = { 4, 1, 2, 0 };

  pc = scp->sc_pc;
  
  if (in_emu) {
    fprintf( stderr, "SIGSEGV in emulator code (pc=%#lx); "
			 "exiting\n", pc );
	rexit( 1 );
  }
  in_emu = 1;
  vecnum = (vecnum & 0xfff) >> 2;
  
  /* nothing to emulate on odd addresses... */
  if (vecnum == VEC_ADDRERR) {
    bombs( vecnum );
  }

  switch( format ) {
  case 0xa:
  case 0xb:
	ssw = framedata->fmta.ssw;
    ea = framedata->fmta.daddr;
	if (ssw & SSW_RMW) {
      fprintf( stderr, "rmw cycles not implemented "
			   "(ea=%08lx, pc=%08lx, code=%04x)\n",
			   ea, pc, *(ushort *)pc );
      bombs( 2 );
    }
	if (ssw & (SSW_FB|SSW_FC)) {
      fprintf( stderr, "instruction read access error cannot be emulated "
			   "(ea=%08lx)\n", ea );
      bombs( 2 );
    }
    size = code2size_030[ (ssw & SSW_SZ) >> 4 ];
    if (!size) {
      fprintf( stderr, "weird size code %lx\n", (ssw & SSW_SZ) >> 4 );
      bombs( 2 );
    }
    if (!(read = (ssw & RW))) {
	  /* failed write, both formats ok */
      value = framedata->fmta.dobuf;
    }
	else {
	  if (format != 0xb) {
		fprintf( stderr, "weird: format 0xa read bus error?!?\n" );
		bombs( 2 );
	  }
	}
    break;

  default:
    fprintf( stderr, "unknown frame format 0x%x\n", format );
    bombs( 2 );
  }

  DDEBUG( "Hardware register access ea=0x%08lx size=%d access=%s pc=%08lx\n",
		  ea, size, read ? "read" : "write", pc );
  if (!read) DDEBUG( "  value=0x%08lx\n", value );

  /*
   *  Here is the actual emulation
   */
  if( !dispatch_hwreg_emu( ea, size, read, (ulong *)&value ) ) {
	DDEBUG( "Emulation of hw register access not possible!\n" );
    bombs( 2 );
  }
  
  if (read) DDEBUG( "  read value=0x%08lx\n", value );

  if( read && format == 0xb ) {
	/* store back emulated value in data input buffer */
    framedata->fmtb.dibuf = value;
  }
  /* clear DF bit in SSW to _not_ rerun the failed cycle */
  framedata->fmta.ssw &= ~SSW_DF;
  in_emu = 0;
}

void bombs( int num )
{
  printf( "Error in TOS program, %d bombs!\n", num );
  rexit( 1 );
}

/* Local Variables:              */
/* tab-width: 4                  */
/* End:                          */
