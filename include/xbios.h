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

#ifndef XBIOS_CALLS_H
#define XBIOS_CALLS_H

#include "div.h"

XBIOSFUNC(Initmous);
XBIOSFUNC(Ssbrk);
XBIOSFUNC(Physbase);
XBIOSFUNC(Logbase);
XBIOSFUNC(Getrez);
XBIOSFUNC(Setscreen);
XBIOSFUNC(Setpalette);
XBIOSFUNC(Setcolor);
XBIOSFUNC(Floprd);
XBIOSFUNC(Flopwr);
XBIOSFUNC(Flopfmt);
XBIOSFUNC(Dbmsg);
XBIOSFUNC(Midiws);
XBIOSFUNC(Mfpint);
XBIOSFUNC(Iorec);
XBIOSFUNC(Rsconf);
XBIOSFUNC(Keytbl);
XBIOSFUNC(Random);
XBIOSFUNC(Protobt);
XBIOSFUNC(Flopver);
XBIOSFUNC(Scrdump);
XBIOSFUNC(Cursconf);
XBIOSFUNC(Settime);
XBIOSFUNC(Gettime);
XBIOSFUNC(Bioskeys);
XBIOSFUNC(Ikbdws);
XBIOSFUNC(Jdisint);
XBIOSFUNC(Jenabint);
XBIOSFUNC(Giaccess);
XBIOSFUNC(Offgibit);
XBIOSFUNC(Ongibit);
XBIOSFUNC(Xbtimer);
XBIOSFUNC(Dosound);
XBIOSFUNC(Setprt);
XBIOSFUNC(Kbdvbase);
XBIOSFUNC(Kbrate);
XBIOSFUNC(Prtblk);
XBIOSFUNC(Vsync);
XBIOSFUNC(Supexec);
XBIOSFUNC(Puntaes);
XBIOSFUNC(Floprate);
XBIOSFUNC(DMAread);
XBIOSFUNC(DMAwrite);
XBIOSFUNC(Bconmap);
XBIOSFUNC(NVMaccess);
XBIOSFUNC(Metainit);
XBIOSFUNC(Blitmode);
XBIOSFUNC(EsetShift);
XBIOSFUNC(EgetShift);
XBIOSFUNC(EsetBank);
XBIOSFUNC(EsetColor);
XBIOSFUNC(EsetPalette);
XBIOSFUNC(EgetPalette);
XBIOSFUNC(EsetGray);
XBIOSFUNC(EsetSmear);
XBIOSFUNC(Vsetmode);
XBIOSFUNC(VgetMonitor);
XBIOSFUNC(VsetSync);
XBIOSFUNC(VgetSize);
XBIOSFUNC(VsetMask);
XBIOSFUNC(VsetRGB);
XBIOSFUNC(VgetRGB);
XBIOSFUNC(Dsp_DoBlock);
XBIOSFUNC(Dsp_BlkHandshake);
XBIOSFUNC(Dsp_BlkUnpacked);
XBIOSFUNC(Dsp_InStream);
XBIOSFUNC(Dsp_Outstream);
XBIOSFUNC(Dsp_IOStream);
XBIOSFUNC(Dsp_RemoveInterrupts);
XBIOSFUNC(Dsp_GetWordSize);
XBIOSFUNC(Dsp_Lock);
XBIOSFUNC(Dsp_Unlock);
XBIOSFUNC(Dsp_Available);
XBIOSFUNC(Dsp_Reserve);
XBIOSFUNC(Dsp_LoadProg);
XBIOSFUNC(Dsp_ExecProg);
XBIOSFUNC(Dsp_ExecBoot);
XBIOSFUNC(Dsp_LodToBinary);
XBIOSFUNC(Dsp_TriggerHC);
XBIOSFUNC(Dsp_RequestUniqueAbility);
XBIOSFUNC(Dsp_GetProgAbility);
XBIOSFUNC(Dsp_FlushSubroutines);
XBIOSFUNC(Dsp_LoadSubroutine);
XBIOSFUNC(Dsp_InqSubrAbility);
XBIOSFUNC(Dsp_RunSubroutine);
XBIOSFUNC(Dsp_Hf0);
XBIOSFUNC(Dsp_Hf1);
XBIOSFUNC(Dsp_Hf2);
XBIOSFUNC(Dsp_Hf3);
XBIOSFUNC(Dsp_BlkWords);
XBIOSFUNC(Dsp_BlkBytes);
XBIOSFUNC(Dsp_Hstat);
XBIOSFUNC(Dsp_SetVectors);
XBIOSFUNC(Dsp_MultBlocks);
XBIOSFUNC(Locksnd);
XBIOSFUNC(Unlocksnd);
XBIOSFUNC(Soundcmd);
XBIOSFUNC(Setbuffer);
XBIOSFUNC(Setmode);
XBIOSFUNC(Settracks);
XBIOSFUNC(Setmontracks);
XBIOSFUNC(Setinterrupt);
XBIOSFUNC(Buffoper);
XBIOSFUNC(Dsptristate);
XBIOSFUNC(Gpio);
XBIOSFUNC(Devconnect);
XBIOSFUNC(Sndstatus);
XBIOSFUNC(Buffptr);

unsigned long dispatch_xbios( char * );

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
