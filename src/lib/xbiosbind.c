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
#include "xbios.h"
#include "bindproto.h"

typedef void voidfunc(void);
typedef long longfunc(void);

/* Fine.. I'm lazy, so sue me! */
#define SBUFPTR void
#define DSPBLOCK void
#define IOREC void
#define KBDVECS void
#define KEYTAB void
#define METAINFO void
#define PRTBLK void
#define RGB void

bindproto1(xbios,long,Bconmap,0x2c,short);
bindproto0v(xbios,Bioskeys,0x18);
bindproto1(xbios,short,Blitmode,0x40,short);
bindproto1(xbios,long,Buffoper,0x88,short);
bindproto1(xbios,long,Buffptr,0x8d,SBUFPTR *);
bindproto2(xbios,short,Cursconf,0x15,short,short);
bindproto3v(xbios,Dbmsg,0x0b,short,short,long);
bindproto5(xbios,long,Devconnect,0x8b,short,short,short,short,short);
bindproto4(xbios,long,DMAread,0x2a,long,short,void *,short);
bindproto4(xbios,long,DMAwrite,0x2b,long,short,void *,short);
bindproto1v(xbios,Dosound,0x20,char *);
bindproto2v(xbios,Dsp_Available,0x6a,long *,long *);
bindproto4v(xbios,Dsp_BlkBytes,0x7c,unsigned char *,long,unsigned char *,long);
bindproto4v(xbios,Dsp_BlkHandShake,0x61,char *,long,char *,long);
bindproto4v(xbios,Dsp_BlkUnpacked,0x62,long *,long,long *,long);
bindproto4v(xbios,Dsp_BlkWords,0x7b,short *,long,short *,long);
bindproto4v(xbios,Dsp_DoBlock,0x60,char *,long,char *,long);
bindproto3v(xbios,Dsp_ExecBoot,0x6e,char *,long,short);
bindproto3v(xbios,Dsp_ExecProg,0x6d,char *,long,short);
bindproto0v(xbios,Dsp_FlushSubroutines,0x73);
bindproto0(xbios,short,Dsp_GetProgAbility,0x72);
bindproto0(xbios,short,Dsp_GetWordSize,0x67);
bindproto1(xbios,short,Dsp_Hf0,0x77,short);
bindproto1(xbios,short,Dsp_Hf1,0x78,short);
bindproto0(xbios,short,Dsp_Hf2,0x79);
bindproto0(xbios,short,Dsp_Hf3,0x7a);
bindproto0(xbios,char,Dsp_HStat,0x7d);
bindproto1(xbios,short,Dsp_InqSubrAbility,0x75,short);
bindproto4v(xbios,Dsp_InStream,0x63,char *,long,long,long *);
bindproto6v(xbios,Dsp_IOStream,0x65,char *,char *,long,long,long,long *);
bindproto3(xbios,short,Dsp_LoadProg,0x6c,char *,short,char *);
bindproto3(xbios,short,Dsp_LoadSubroutine,0x74,char *,long,short);
bindproto0(xbios,short,Dsp_Lock,0x68);
bindproto2(xbios,long,Dsp_LodToBinary,0x6f,char *,char *);
bindproto4v(xbios,Dsp_MultBlocks,0x7f,long,long,DSPBLOCK *,DSPBLOCK *);
bindproto4v(xbios,Dsp_OutStream,0x64,char *,long,long,long *);
bindproto1v(xbios,Dsp_RemoveInterrupts,0x66,short);
bindproto0(xbios,short,Dsp_RequestUniqueAbility,0x71);
bindproto2(xbios,short,Dsp_Reserve,0x6b,long,long);
bindproto1(xbios,short,Dsp_RunSubroutine,0x76,short);
bindproto2v(xbios,Dsp_SetVectors,0x7e,voidfunc *,longfunc *);
bindproto1v(xbios,Dsp_TriggerHC,0x70,short);
bindproto0v(xbios,Dsp_Unlock,0x69);
bindproto2(xbios,long,Dsptristate,0x89,short,short);
bindproto3v(xbios,EgetPalette,0x55,short,short,short *);
bindproto0(xbios,short,EgetShift,0x51);
bindproto1(xbios,short,EsetBank,0x52,short);
bindproto2(xbios,short,EsetColor,0x53,short,short);
bindproto1(xbios,short,EsetGray,0x56,short);
bindproto3v(xbios,EsetPalette,0x54,short,short,short *);
bindproto1(xbios,short,EsetShift,0x50,short);
bindproto1(xbios,short,EsetSmear,0x57,short);
bindproto9(xbios,short,Flopfmt,0x0a,void *,short *,short,short,short,short,short,long,short);
bindproto2(xbios,short,Floprate,0x29,short,short);
bindproto7(xbios,short,Floprd,0x08,void *,long,short,short,short,short,short);
bindproto7(xbios,short,Flopver,0x13,void *,long,short,short,short,short,short);
bindproto7(xbios,short,Flopwr,0x09,void *,long,short,short,short,short,short);
bindproto0(xbios,short,Getrez,0x04);
bindproto0(xbios,long,Gettime,0x17);
bindproto2(xbios,short,Giaccess,0x1c,short,short);
bindproto2(xbios,long,Gpio,0x8a,short,short);
bindproto2v(xbios,Ikbdws,0x19,short,char *);
bindproto3v(xbios,Initmous,0x00,short,void *,voidfunc *);
bindproto1(xbios,IOREC *,Iorec,0x0e,short);
bindproto1v(xbios,Jdisint,0x1a,short);
bindproto1v(xbios,Jenabint,0x1b,short);
bindproto0(xbios,KBDVECS *,Kbdvbase,0x22);
bindproto2(xbios,short,Kbrate,0x23,short,short);
bindproto3(xbios,KEYTAB *,Keytbl,0x10,char *,char *,char *);
bindproto0(xbios,long,Locksnd,0x80);
bindproto0(xbios,void *,Logbase,0x03);
bindproto1v(xbios,Metainit,0x30,METAINFO *);
bindproto2v(xbios,Mfpint,0x0d,short,voidfunc *);
bindproto2v(xbios,Midiws,0x0c,short,char *);
bindproto4(xbios,short,NVMaccess,0x2e,short,short,short,char *);
bindproto1v(xbios,Offgibit,0x1d,short);
bindproto1v(xbios,Ongibit,0x1e,short);
bindproto0(xbios,void *,Physbase,0x02);
bindproto4v(xbios,Protobt,0x12,void *,long,short,short);
bindproto1(xbios,short,Prtblk,0x24,PRTBLK *);
bindproto0v(xbios,Puntaes,0x27);
bindproto0(xbios,long,Random,0x11);
bindproto6(xbios,unsigned long,Rsconf,0x0f,short,short,short,short,short,short);
bindproto0v(xbios,Scrdmp,0x14);
bindproto3(xbios,long,Setbuffer,0x83,short,void *,void *);
bindproto2(xbios,short,Setcolor,0x07,short,short);
bindproto2(xbios,long,Setinterrupt,0x87,short,short);
bindproto1(xbios,long,Setmode,0x84,short);
bindproto1(xbios,long,Setmontracks,0x86,short);
bindproto1v(xbios,Setpalette,0x06,short *);
bindproto1(xbios,short,Setprt,0x21,short);
bindproto3v(xbios,Setscreen,0x05,void *,void *,short);
bindproto1v(xbios,Settime,0x16,long);
bindproto2(xbios,long,Settracks,0x85,short,short);
bindproto1(xbios,long,Sndstatus,0x8c,short);
bindproto2(xbios,long,Soundcmd,0x82,short,short);
bindproto1(xbios,void *,Ssbrk,0x01,short);
bindproto1(xbios,long,Supexec,0x26,longfunc *);
bindproto0(xbios,long,Unlocksnd,0x81);
bindproto0(xbios,short,VgetMonitor,0x59);
bindproto3v(xbios,VgetRGB,0x5e,short,short,RGB *);
bindproto1(xbios,long,VgetSize,0x5b,short);
bindproto3v(xbios,VsetMask,0x92,long,long,short);
bindproto1(xbios,short,VsetMode,0x58,short);
bindproto3v(xbios,VsetRGB,0x5d,short,short,RGB *);
bindproto4v(xbios,VsetScreen,0x05,void *,void *,short,short);
bindproto1v(xbios,VsetSync,0x5a,short);
bindproto0v(xbios,Vsync,0x25);
bindproto4v(xbios,Xbtimer,0x1f,short,short,short,voidfunc *);



