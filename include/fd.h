/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
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

#ifndef _gemdos_fd_h
#define _gemdos_fd_h

/* max. GEMDOS file handle */
#define MAX_GEMDOS_FD	64

#define MIN_UNIX_FD		3
#define MAX_UNIX_FD		75

/* standard GEMDOS handles (0..6) */
#define GEMDOS_STDIN	0
#define GEMDOS_STDOUT	1
#define GEMDOS_STDAUX	2
#define GEMDOS_STDPRN	3
#define GEMDOS_STD4		4
#define GEMDOS_STD5		5

#define GEMDOS_FIRST_NONSTD	6

/* GEMDOS device handles */
#define GEMDOS_DEV_CON	(-1)
#define GEMDOS_DEV_AUX	(-2)
#define GEMDOS_DEV_PRN	(-3)

/*
 * Array to map GEMDOS file handles to Unix fd's
 * 
 * Possible values are:
 *   < 0 : GEMDOS hard device (I/O handleded via BIOS)
 *   >= 0: standard Unix fd
 *   HANDLE_FREE: handle is not in use
 *
 * For the device handles, -3..-1 are used as the real GEMDOS does:
 *   -1 = CON:, -2 = AUX, -3 = PRN
 * -4 and below are internally assigned by the emulator for other BIOS
 * devices, in preparation for U:\DEV\MODEM2 and the like. The mapping is
 *   BIOSdev = -GEMDOSh - 1
 */
extern int FdMap[];

/*
 * Reference counters for Unix fd's; to know when to close them
 * (does not apply to fd's 0..2)
 */
extern int FdCnt[];

/* constants and predicates for FdMap */
#define HANDLE_FREE		0x7fffffff /* not allocated */

#define IS_FREE_HANDLE(h)	((h) == HANDLE_FREE)
#define IS_DEV_HANDLE(h)	((h) <= GEMDOS_DEV_CON)
#define IS_STD_HANDLE(h)	((h) >= GEMDOS_STDIN && (h) <= GEMDOS_STD5)
#define IS_NONSTD_HANDLE(h)	((h) >= GEMDOS_FIRST_NONSTD && (h) < MAX_GEMDOS_FD)

/* convenience macros for handle translation (GEMDOS->Unix); one version if
 * device handles are allowed, one for if not */
#define GETFD(fd)		if (!getfd( &fd )) return TOS_EIHNDL;
#define GETFD_NODEV(fd)	if (!getfd( &fd ) || fd < 0) return TOS_EIHNDL;

/* map GEMDOS device handle (negative) to BIOS device number */
#define BIOSDEV(fd)		((fd) >= GEMDOS_DEV_PRN ? ((fd) + 3) : (-(fd)-1))

int getfd( int *fd );
int new_handle( void );

#endif  /* _gemdos_fd_h */

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
