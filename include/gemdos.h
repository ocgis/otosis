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

#ifndef GEMDOS_CALLS_H
#define GEMDOS_CALLS_H

GEMDOSFUNC(Pterm0);
GEMDOSFUNC(Cconin);
GEMDOSFUNC(Cconout);
GEMDOSFUNC(Cauxin);
GEMDOSFUNC(Cauxout);
GEMDOSFUNC(Cprnout);
GEMDOSFUNC(Crawio);
GEMDOSFUNC(Crawcin);
GEMDOSFUNC(Cnecin);
GEMDOSFUNC(Cconws);
GEMDOSFUNC(Cconrs);
GEMDOSFUNC(Cconis);
GEMDOSFUNC(Dsetdrv);
GEMDOSFUNC(Cconos);
GEMDOSFUNC(Cprnos);
GEMDOSFUNC(Cauxis);
GEMDOSFUNC(Cauxos);
GEMDOSFUNC(Maddalt);
GEMDOSFUNC(Dgetdrv);
GEMDOSFUNC(Fsetdta);
GEMDOSFUNC(Super);
GEMDOSFUNC(Tgetdate);
GEMDOSFUNC(Tsetdate);
GEMDOSFUNC(Tgettime);
GEMDOSFUNC(Tsettime);
GEMDOSFUNC(Fgetdta);
GEMDOSFUNC(Sversion);
GEMDOSFUNC(Ptermres);
GEMDOSFUNC(Dfree);
GEMDOSFUNC(Dcreate);
GEMDOSFUNC(Ddelete);
GEMDOSFUNC(Dsetpath);
GEMDOSFUNC(Fcreate);
GEMDOSFUNC(Fopen);
GEMDOSFUNC(Fclose);
GEMDOSFUNC(Fread);
GEMDOSFUNC(Fwrite);
GEMDOSFUNC(Fdelete);
GEMDOSFUNC(Fseek);
GEMDOSFUNC(Fattrib);
GEMDOSFUNC(Mxalloc);
GEMDOSFUNC(Fdup);
GEMDOSFUNC(Fforce);
GEMDOSFUNC(Dgetpath);
GEMDOSFUNC(Malloc);
GEMDOSFUNC(Mfree);
GEMDOSFUNC(Mshrink);
GEMDOSFUNC(Pexec);
GEMDOSFUNC(Pterm);
GEMDOSFUNC(Fsfirst);
GEMDOSFUNC(Fsnext);
GEMDOSFUNC(Frename);
GEMDOSFUNC(Fdatime);
GEMDOSFUNC(Flock);

unsigned long dispatch_gemdos( char * );
void init_gemdos( void );

/* utility functions */
long ioready( int fd, int mode );
#define DO_NOTHING	0
#define DO_ECHO		1
#define DO_CTRL		2
long generic_instat( int fd );
long generic_outstat( int fd );
int gemdos_readstr( int fd, char *dest, int max );
long generic_cout( int fd, int c );
long generic_cin( int fd, int flags );

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
