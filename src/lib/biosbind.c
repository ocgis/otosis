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
#include "bios.h"
#include "bindproto.h"

typedef void voidfunc(void);

bindproto1(bios,long,Bconin,0x02,short);
bindproto1(bios,long,Bconout,0x03,short);
bindproto1(bios,long,Bconstat,0x01,short);
bindproto1(bios,long,Bcostat,0x08,short);
bindproto0(bios,unsigned long,Drvmap,0x0a);
bindproto1(bios,Bpb *,Getbpb,0x07,short);
bindproto1v(bios,Getmpb,0x00,void *);
bindproto1(bios,long,Kbshift,0x0b,short);
bindproto1(bios,long,Mediach,0x09,short);
bindproto6(bios,long,Rwabs,0x04,short,void *,short,short,short,long);
bindproto2(bios,voidfunc *,Setexc,0x05,short,voidfunc *);
bindproto0(bios,long,Tickcal,0x06);

