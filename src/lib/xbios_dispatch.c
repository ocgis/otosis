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
#include "toserrors.h"
#include "xbios.h"

#define	UNUSED	(TosSystemCall *)NULL

extern TosProgram *prog;

TosSystemCall *xbios_syscalls[] = {
/*   0 */  xbios_Initmous, xbios_Ssbrk, xbios_Physbase, xbios_Logbase, xbios_Getrez,
/*   5 */  xbios_Setscreen, xbios_Setpalette, xbios_Setcolor, xbios_Floprd, xbios_Flopwr,
/*  10 */  xbios_Flopfmt, xbios_Dbmsg, xbios_Midiws, xbios_Mfpint, xbios_Iorec,
/*  15 */  xbios_Rsconf, xbios_Keytbl, xbios_Random, xbios_Protobt, xbios_Flopver,
/*  20 */  xbios_Scrdump, xbios_Cursconf, xbios_Settime, xbios_Gettime, xbios_Bioskeys,
/*  25 */  xbios_Ikbdws, xbios_Jdisint, xbios_Jenabint, xbios_Giaccess, xbios_Offgibit,
/*  30 */  xbios_Ongibit, xbios_Xbtimer, xbios_Dosound, xbios_Setprt, xbios_Kbdvbase,
/*  35 */  xbios_Kbrate, xbios_Prtblk, xbios_Vsync, xbios_Supexec, xbios_Puntaes,
/*  40 */  UNUSED, xbios_Floprate, xbios_DMAread, xbios_DMAwrite, xbios_Bconmap,
/*  45 */  UNUSED, xbios_NVMaccess, UNUSED, xbios_Metainit, UNUSED,
/*  50 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  55 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  60 */  UNUSED, UNUSED, UNUSED, UNUSED, xbios_Blitmode,
/*  65 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  70 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  75 */  UNUSED, UNUSED, UNUSED, UNUSED, UNUSED,
/*  80 */  xbios_EsetShift, xbios_EgetShift, xbios_EsetBank, xbios_EsetColor, xbios_EsetPalette,
/*  85 */  xbios_EgetPalette, xbios_EsetGray, xbios_EsetSmear, xbios_Vsetmode, xbios_VgetMonitor,
/*  90 */  xbios_VsetSync, xbios_VgetSize, xbios_VsetMask, xbios_VsetRGB, xbios_VgetRGB,
/*  95 */  UNUSED, xbios_Dsp_DoBlock, xbios_Dsp_BlkHandshake, xbios_Dsp_BlkUnpacked, xbios_Dsp_InStream,
/* 100 */  xbios_Dsp_Outstream, xbios_Dsp_IOStream, xbios_Dsp_RemoveInterrupts, xbios_Dsp_GetWordSize, xbios_Dsp_Lock,
/* 105 */  xbios_Dsp_Unlock, xbios_Dsp_Available, xbios_Dsp_Reserve, xbios_Dsp_LoadProg, xbios_Dsp_ExecProg,
/* 110 */  xbios_Dsp_ExecBoot, xbios_Dsp_LodToBinary, xbios_Dsp_TriggerHC, xbios_Dsp_RequestUniqueAbility, xbios_Dsp_GetProgAbility,
/* 115 */  xbios_Dsp_FlushSubroutines, xbios_Dsp_LoadSubroutine, xbios_Dsp_InqSubrAbility, xbios_Dsp_RunSubroutine, xbios_Dsp_Hf0,
/* 120 */  xbios_Dsp_Hf1, xbios_Dsp_Hf2, xbios_Dsp_Hf3, xbios_Dsp_BlkWords, xbios_Dsp_BlkBytes,
/* 125 */  xbios_Dsp_Hstat, xbios_Dsp_SetVectors, xbios_Dsp_MultBlocks, xbios_Locksnd, xbios_Unlocksnd,
/* 130 */  xbios_Soundcmd, xbios_Setbuffer, xbios_Setmode, xbios_Settracks, xbios_Setmontracks,
/* 135 */  xbios_Setinterrupt, xbios_Buffoper, xbios_Dsptristate, xbios_Gpio, xbios_Devconnect,
/* 140 */  xbios_Sndstatus, xbios_Buffptr,
};

#ifdef DEBUG
char *xbios_call_names[] = {
  "Initmous", "Ssbrk", "Physbase", "Logbase", "Getrez",
  "Setscreen", "Setpalette", "Setcolor", "Floprd", "Flopwr",
  "Flopfmt", "Dbmsg", "Midiws", "Mfpint", "Iorec",
  "Rsconf", "Keytbl", "Random", "Protobt", "Flopver",
  "Scrdump", "Cursconf", "Settime", "Gettime", "Bioskeys",
  "Ikbdws", "Jdisint", "Jenabint", "Giaccess", "Offgibit",
  "Ongibit", "Xbtimer", "Dosound", "Setprt", "Kbdvbase",
  "Kbrate", "Prtblk", "Vsync", "Supexec", "Puntaes",
  "unused", "Floprate", "DMAread", "DMAwrite", "Bconmap",
  "unused", "NVMaccess", "unused", "Metainit", "unused",
  "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "Blitmode",
  "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused",
  "unused", "unused", "unused", "unused", "unused",
  "EsetShift", "EgetShift", "EsetBank", "EsetColor", "EsetPalette",
  "EgetPalette", "EsetGray", "EsetSmear", "Vsetmode", "VgetMonitor",
  "VsetSync", "VgetSize", "VsetMask", "VsetRGB", "VgetRGB",
  "unused", "Dsp_DoBlock", "Dsp_BlkHandshake", "Dsp_BlkUnpacked",
  "Dsp_InStream", "Dsp_Outstream", "Dsp_IOStream", "Dsp_RemoveInterrupts",
  "Dsp_GetWordSize", "Dsp_Lock", "Dsp_Unlock", "Dsp_Available", "Dsp_Reserve",
  "Dsp_LoadProg", "Dsp_ExecProg", "Dsp_ExecBoot", "Dsp_LodToBinary",
  "Dsp_TriggerHC", "Dsp_RequestUniqueAbility", "Dsp_GetProgAbility",
  "Dsp_FlushSubroutines", "Dsp_LoadSubroutine", "Dsp_InqSubrAbility",
  "Dsp_RunSubroutine", "Dsp_Hf0", "Dsp_Hf1", "Dsp_Hf2", "Dsp_Hf3",
  "Dsp_BlkWords", "Dsp_BlkBytes", "Dsp_Hstat", "Dsp_SetVectors",
  "Dsp_MultBlocks", "Locksnd", "Unlocksnd", "Soundcmd", "Setbuffer",
  "Setmode", "Settracks", "Setmontracks", "Setinterrupt", "Buffoper",
  "Dsptristate", "Gpio", "Devconnect", "Sndstatus", "Buffptr"
};

/* Some formats still need to be filled in, since 1) I don't have docs for the
 * Falcon-specific calls, or 2) am too lazy :-) */
char *xbios_call_args[] = {
  "Initmous", "Ssbrk", "Physbase", "Logbase", "Getrez",
  "Setscreen", "Setpalette", "Setcolor", "Floprd", "Flopwr",
  "Flopfmt", "Dbmsg", "Midiws", "Mfpint", "Iorec",
  "Rsconf", "Keytbl", "Random", "Protobt", "Flopver",
  "Scrdump", "Cursconf", "Settime", "Gettime", "Bioskeys",
  "Ikbdws", "Jdisint", "Jenabint", "Giaccess", "Offgibit",
  "Ongibit", "Xbtimer", "Dosound", "Setprt", "Kbdvbase",
  "Kbrate", "Prtblk", "Vsync", "%08lx", "Puntaes",
  NULL, "Floprate", "DMAread", "DMAwrite", "Bconmap",
  NULL, "NVMaccess", NULL, "Metainit", NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, "Blitmode",
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  "EsetShift", "EgetShift", "EsetBank", "EsetColor", "EsetPalette",
  "EgetPalette", "EsetGray", "EsetSmear", "Vsetmode", "VgetMonitor",
  "VsetSync", "VgetSize", "VsetMask", "VsetRGB", "VgetRGB",
  NULL, "Dsp_DoBlock", "Dsp_BlkHandshake", "Dsp_BlkUnpacked",
  "Dsp_InStream", "Dsp_Outstream", "Dsp_IOStream", "Dsp_RemoveInterrupts",
  "Dsp_GetWordSize", "Dsp_Lock", "Dsp_Unlock", "Dsp_Available", "Dsp_Reserve",
  "Dsp_LoadProg", "Dsp_ExecProg", "Dsp_ExecBoot", "Dsp_LodToBinary",
  "Dsp_TriggerHC", "Dsp_RequestUniqueAbility", "Dsp_GetProgAbility",
  "Dsp_FlushSubroutines", "Dsp_LoadSubroutine", "Dsp_InqSubrAbility",
  "Dsp_RunSubroutine", "Dsp_Hf0", "Dsp_Hf1", "Dsp_Hf2", "Dsp_Hf3",
  "Dsp_BlkWords", "Dsp_BlkBytes", "Dsp_Hstat", "Dsp_SetVectors",
  "Dsp_MultBlocks", "Locksnd", "Unlocksnd", "Soundcmd", "Setbuffer",
  "Setmode", "Settracks", "Setmontracks", "Setinterrupt", "Buffoper",
  "Dsptristate", "Gpio", "Devconnect", "Sndstatus", "Buffptr"
};

/* Don't use non-32-bit formats for first arg!! */
char *xbios_call_retv[] = {
  "Initmous", "Ssbrk", "Physbase", "Logbase", "Getrez",
  "Setscreen", "Setpalette", "Setcolor", "Floprd", "Flopwr",
  "Flopfmt", "Dbmsg", "Midiws", "Mfpint", "Iorec",
  "Rsconf", "Keytbl", "Random", "Protobt", "Flopver",
  "Scrdump", "Cursconf", "Settime", "Gettime", "Bioskeys",
  "Ikbdws", "Jdisint", "Jenabint", "Giaccess", "Offgibit",
  "Ongibit", "Xbtimer", "Dosound", "Setprt", "Kbdvbase",
  "Kbrate", "Prtblk", "Vsync", "%08lx", "Puntaes",
  NULL, "Floprate", "DMAread", "DMAwrite", "Bconmap",
  NULL, "NVMaccess", NULL, "Metainit", NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, "Blitmode",
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, NULL, NULL, NULL,
  "EsetShift", "EgetShift", "EsetBank", "EsetColor", "EsetPalette",
  "EgetPalette", "EsetGray", "EsetSmear", "Vsetmode", "VgetMonitor",
  "VsetSync", "VgetSize", "VsetMask", "VsetRGB", "VgetRGB",
  NULL, "Dsp_DoBlock", "Dsp_BlkHandshake", "Dsp_BlkUnpacked",
  "Dsp_InStream", "Dsp_Outstream", "Dsp_IOStream", "Dsp_RemoveInterrupts",
  "Dsp_GetWordSize", "Dsp_Lock", "Dsp_Unlock", "Dsp_Available", "Dsp_Reserve",
  "Dsp_LoadProg", "Dsp_ExecProg", "Dsp_ExecBoot", "Dsp_LodToBinary",
  "Dsp_TriggerHC", "Dsp_RequestUniqueAbility", "Dsp_GetProgAbility",
  "Dsp_FlushSubroutines", "Dsp_LoadSubroutine", "Dsp_InqSubrAbility",
  "Dsp_RunSubroutine", "Dsp_Hf0", "Dsp_Hf1", "Dsp_Hf2", "Dsp_Hf3",
  "Dsp_BlkWords", "Dsp_BlkBytes", "Dsp_Hstat", "Dsp_SetVectors",
  "Dsp_MultBlocks", "Locksnd", "Unlocksnd", "Soundcmd", "Setbuffer",
  "Setmode", "Settracks", "Setmontracks", "Setinterrupt", "Buffoper",
  "Dsptristate", "Gpio", "Devconnect", "Sndstatus", "Buffptr"
};
#endif

unsigned long dispatch_xbios(char * _args)
{
  TOSARG(short, callnum);
  long rv;

  if (callnum < 0 || callnum > arraysize(xbios_syscalls) ||
	  !xbios_syscalls[callnum]) {
    DDEBUG( "call to invalid XBIOS function #%d\n", callnum );
    return TOS_EINVFN;
  }

  STRACE_BEGIN(xbios, _args);
  rv = xbios_syscalls[callnum](_args);
  STRACE_END(xbios, _args, rv);
  return rv;
}

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
