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
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "div.h"
#include "prototypes.h"
#include "xbios.h"
#include "toserrors.h"

extern TosProgram *prog;

XBIOS_UNIMP(Initmous);
XBIOS_UNIMP(Ssbrk);
XBIOS_UNIMP(Physbase);
XBIOS_UNIMP(Logbase);
XBIOS_UNIMP(Getrez);
XBIOS_UNIMP(Setscreen);
XBIOS_UNIMP(Setpalette);
XBIOS_UNIMP(Setcolor);
XBIOS_UNIMP(Floprd);
XBIOS_UNIMP(Flopwr);
XBIOS_UNIMP(Flopfmt);
XBIOS_UNIMP(Dbmsg);
XBIOS_UNIMP(Midiws);
XBIOS_UNIMP(Mfpint);
XBIOS_UNIMP(Iorec);
XBIOS_UNIMP(Rsconf);
XBIOS_UNIMP(Keytbl);
XBIOS_UNIMP(Random);
XBIOS_UNIMP(Protobt);
XBIOS_UNIMP(Flopver);
XBIOS_UNIMP(Scrdump);
XBIOS_UNIMP(Cursconf);
XBIOS_UNIMP(Settime);
XBIOS_UNIMP(Gettime);
XBIOS_UNIMP(Bioskeys);
XBIOS_UNIMP(Ikbdws);
XBIOS_UNIMP(Jdisint);
XBIOS_UNIMP(Jenabint);
XBIOS_UNIMP(Giaccess);
XBIOS_UNIMP(Offgibit);
XBIOS_UNIMP(Ongibit);
XBIOS_UNIMP(Xbtimer);
XBIOS_UNIMP(Dosound);
XBIOS_UNIMP(Setprt);
XBIOS_UNIMP(Kbdvbase);
XBIOS_UNIMP(Kbrate);
XBIOS_UNIMP(Prtblk);
XBIOS_UNIMP(Vsync);

volatile int in_emu = 1;

XBIOSFUNC(Supexec)
{
  TOSARG(char *,addr);
  unsigned long rv;
  int saved_super = prog->super;
  
  DDEBUG( "calling Supexec( %p )\n", addr );
  prog->super = 1;
  in_emu = 0;
  /*
   *  The routine that is called in the inline might destroy other
   *  variables than the ones specified in the clobbered lise
   *
   */
  /* FIXME */
  #ifdef mc68000
  __asm__ __volatile__ ( "moveml %/d3-%/d7/%/a3-%/a6,%/sp@-	\n\t"
			 "jsr %1@ ; movel %/d0,%0		\n\t"
			 "moveml %/sp@+,%/d3-%/d7/%/a3-%/a6" :
			 "=g" (rv) :
			 "a" (addr) :
			 /* all not-call-saved regs must be in clobbered
			  * list! */
			 "d0", "d1", "d2", "a0", "a1", "a2" );
  #endif
  in_emu = 1;
  prog->super = saved_super;
  DDEBUG( "returning from Supexec\n" );
  return rv;
}

XBIOS_UNIMP(Puntaes);
XBIOS_UNIMP(Floprate);
XBIOS_UNIMP(DMAread);
XBIOS_UNIMP(DMAwrite);
XBIOS_UNIMP(Bconmap);
XBIOS_UNIMP(NVMaccess);
XBIOS_UNIMP(Metainit);
XBIOS_UNIMP(Blitmode);
XBIOS_UNIMP(EsetShift);
XBIOS_UNIMP(EgetShift);
XBIOS_UNIMP(EsetBank);
XBIOS_UNIMP(EsetColor);
XBIOS_UNIMP(EsetPalette);
XBIOS_UNIMP(EgetPalette);
XBIOS_UNIMP(EsetGray);
XBIOS_UNIMP(EsetSmear);
XBIOS_UNIMP(Vsetmode);
XBIOS_UNIMP(VgetMonitor);
XBIOS_UNIMP(VsetSync);
XBIOS_UNIMP(VgetSize);
XBIOS_UNIMP(VsetMask);
XBIOS_UNIMP(VsetRGB);
XBIOS_UNIMP(VgetRGB);
XBIOS_UNIMP(Dsp_DoBlock);
XBIOS_UNIMP(Dsp_BlkHandshake);
XBIOS_UNIMP(Dsp_BlkUnpacked);
XBIOS_UNIMP(Dsp_InStream);
XBIOS_UNIMP(Dsp_Outstream);
XBIOS_UNIMP(Dsp_IOStream);
XBIOS_UNIMP(Dsp_RemoveInterrupts);
XBIOS_UNIMP(Dsp_GetWordSize);
XBIOS_UNIMP(Dsp_Lock);
XBIOS_UNIMP(Dsp_Unlock);
XBIOS_UNIMP(Dsp_Available);
XBIOS_UNIMP(Dsp_Reserve);
XBIOS_UNIMP(Dsp_LoadProg);
XBIOS_UNIMP(Dsp_ExecProg);
XBIOS_UNIMP(Dsp_ExecBoot);
XBIOS_UNIMP(Dsp_LodToBinary);
XBIOS_UNIMP(Dsp_TriggerHC);
XBIOS_UNIMP(Dsp_RequestUniqueAbility);
XBIOS_UNIMP(Dsp_GetProgAbility);
XBIOS_UNIMP(Dsp_FlushSubroutines);
XBIOS_UNIMP(Dsp_LoadSubroutine);
XBIOS_UNIMP(Dsp_InqSubrAbility);
XBIOS_UNIMP(Dsp_RunSubroutine);
XBIOS_UNIMP(Dsp_Hf0);
XBIOS_UNIMP(Dsp_Hf1);
XBIOS_UNIMP(Dsp_Hf2);
XBIOS_UNIMP(Dsp_Hf3);
XBIOS_UNIMP(Dsp_BlkWords);
XBIOS_UNIMP(Dsp_BlkBytes);
XBIOS_UNIMP(Dsp_Hstat);
XBIOS_UNIMP(Dsp_SetVectors);
XBIOS_UNIMP(Dsp_MultBlocks);
XBIOS_UNIMP(Locksnd);
XBIOS_UNIMP(Unlocksnd);
XBIOS_UNIMP(Soundcmd);
XBIOS_UNIMP(Setbuffer);
XBIOS_UNIMP(Setmode);
XBIOS_UNIMP(Settracks);
XBIOS_UNIMP(Setmontracks);
XBIOS_UNIMP(Setinterrupt);
XBIOS_UNIMP(Buffoper);
XBIOS_UNIMP(Dsptristate);
XBIOS_UNIMP(Gpio);
XBIOS_UNIMP(Devconnect);
XBIOS_UNIMP(Sndstatus);
XBIOS_UNIMP(Buffptr);

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
