/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
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

#include <stdio.h>
#include <stdlib.h>

#include <vdibind.h>
#include <aesbind.h>

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "xgemdos.h"

/* Should call PTerm(0), but this is the same thing */
XGEMDOSFUNC(Warmstart)
{
  DDEBUG("\n");
  rexit( 0 );
}

XGEMDOSFUNC(VDI)
{
  TOSARG(ulong,vdi_pb);

  vdi_call( (VDIPB *)vdi_pb );
  
  return 0;
}

XGEMDOSFUNC(AES)
{
  TOSARG(ulong,aes_pb);

  aes_call( (AESPB *)aes_pb );
  
  return 0;
}

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
