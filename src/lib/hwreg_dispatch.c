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

int dispatch_hwreg_emu( ulong ea, int size, int read, ulong *value )
{
	/* redirect 0x00ffxxxx I/O region to 0xffffxxxx */
	if (ea >= 0x00ff0000 && ea <= 0x00ffffff)
		ea += 0xff000000;
	
	switch( ea ) {
	  case 0xfffffa42:			/* FPU interface */
		if (read) {
			*value = 0;
		}
		else {
			/* ignore it... */
		}
		return 1;
	}
	
	return 0;
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
