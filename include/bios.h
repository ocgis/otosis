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

#ifndef BIOS_CALLS_H
#define BIOS_CALLS_H

#include "div.h"

BIOSFUNC(Getmpb);
BIOSFUNC(Bconstat);
BIOSFUNC(Bconin);
BIOSFUNC(Bconout);
BIOSFUNC(Rwabs);
BIOSFUNC(Setexc);
BIOSFUNC(Tickcal);
BIOSFUNC(Getbpb);
BIOSFUNC(Bcostat);
BIOSFUNC(Mediach);
BIOSFUNC(Drvmap);
BIOSFUNC(Kbshift);

long internal_Bconstat( int dev );
long internal_Bconin( int dev );
long internal_Bconout( int dev, int c );
long internal_Bcostat( int dev );
void init_biosdev_fd( void );

unsigned long dispatch_bios( char * );
void init_bios( void );

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
