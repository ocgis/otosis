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
#include "xgemdos.h"

#include <aesbind.h>
#include <vdibind.h>

/* None of these will actually be used if the
 * programmer is at least somewhat sane, but
 * I still included them here.
 */

long AES(AESPB *aespb)
{
  return (long)dispatch_xgemdos(200, (unsigned long)aespb);
}

long VDI(VDIPB *vdipb)
{
  return (long)dispatch_xgemdos(115, (unsigned long)vdipb);
}

long Warmstart(void)
{
  (void)dispatch_xgemdos(0, 0);
}


