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
#include "gemdos.h"
#include "bindproto.h"


bindproto0(gemdos,short,Cauxin,0x03);
bindproto0(gemdos,short,Cauxis,0x12);
bindproto0(gemdos,short,Cauxos,0x13);
bindproto1v(gemdos,Cauxout,0x04,short);
bindproto0(gemdos,long,Cconin,0x01);
bindproto0(gemdos,short,Cconis,0x0b);
bindproto0(gemdos,short,Cconos,0x10);
bindproto1v(gemdos,Cconout,0x02,short);
bindproto1v(gemdos,Cconrs,0x0a,char *);
bindproto1v(gemdos,Cconws,0x09,char *);
bindproto0(gemdos,short,Cnecin,0x08);
bindproto0(gemdos,short,Cprnos,0x11);
bindproto1(gemdos,short,Cprnout,0x05,short);
bindproto0(gemdos,long,Crawcin,0x07);
bindproto1(gemdos,long,Crawio,0x06,short);
bindproto1(gemdos,long,Dcreate,0x39,char *);
bindproto1(gemdos,long,Ddelete,0x3a,char *);
bindproto2(gemdos,long,Dfree,0x36,Diskinfo *,short);
bindproto0(gemdos,short,Dgetdrv,0x19);
bindproto2(gemdos,long,Dgetpath,0x47,char *,short);
bindproto1(gemdos,long,Dsetdrv,0x0e,short);
bindproto1(gemdos,long,Dsetpath,0x3b,char *);
bindproto3(gemdos,long,Fattrib,0x43,char *,short,short);
bindproto1(gemdos,long,Fclose,0x3e,short);
bindproto2(gemdos,long,Fcreate,0x3c,char *,short);
bindproto3(gemdos,long,Fdatime,0x57,Datetime *,short,short);
bindproto1(gemdos,long,Fdelete,0x41,char *);
bindproto1(gemdos,long,Fdup,0x45,short);
bindproto2(gemdos,long,Fforce,0x46,short,short);
bindproto0(gemdos,Dta *,Fgetdta,0x2f);
bindproto4(gemdos,long,Flock,0x5c,short,short,long,long);
bindproto2(gemdos,long,Fopen,0x3d,char *,short);
bindproto3(gemdos,long,Fread,0x3f,short,long,void *);
bindproto3(gemdos,long,Frename,0x56,short,char *,char *);
bindproto3(gemdos,long,Fseek,0x42,long,short,short);
bindproto1v(gemdos,Fsetdta,0x1a,Dta *);
bindproto2(gemdos,short,Fsfirst,0x4e,char *,short);
bindproto0(gemdos,short,Fsnext,0x4f);
bindproto3(gemdos,long,Fwrite,0x40,short,long,void *);
bindproto2(gemdos,long,Maddalt,0x14,void *,long);
bindproto1(gemdos,void *,Malloc,0x48,long);
bindproto1(gemdos,short,Mfree,0x49,void *);
bindproto2(gemdos,short,Mshrink,0x4a,void *,long);
bindproto2(gemdos,void *,Mxalloc,0x44,long,short);
bindproto4(gemdos,long,Pexec,0x4b,short,char *,char *,char *);
bindproto1v(gemdos,Pterm,0x4c,short);
bindproto0v(gemdos,Pterm0,0x00);
bindproto2v(gemdos,Ptermres,0x31,long,short);
bindproto1(gemdos,void *,Super,0x20,void *);
bindproto0(gemdos,unsigned short,Sversion,0x30);
bindproto0(gemdos,unsigned short,Tgetdate,0x2a);
bindproto0(gemdos,unsigned short,Tgettime,0x2c);
bindproto1(gemdos,short,Tsetdate,0x2b,unsigned short);
bindproto1(gemdos,short,Tsettime,0x2d,unsigned short);




