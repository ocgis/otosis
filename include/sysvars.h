/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
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

#ifndef SYSVARS_H
#define SYSVARS_H

#define SYSVAR_MEMVALID		(*(ulong *)0x420)
#define SYSVAR_PHYSTOP		(*(ulong *)0x42E)
#define SYSVAR_MEMBOT		(*(ulong *)0x432)
#define SYSVAR_MEMTOP		(*(ulong *)0x436)
#define SYSVAR_BOOTDEV		(*(ushort *)0x446)
#define SYSVAR_SYSBASE		(*(ulong *)0x4F2)
#define SYSVAR_COOKIES		(*(ulong *)0x5A0)
#define SYSVAR_RAMTOP		(*(ulong *)0x5A4)
#define SYSVAR_RAMVALID		(*(ulong *)0x5A8)

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
