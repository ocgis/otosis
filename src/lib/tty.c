/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *  Copyright 1998 Tomas Berndtsson <tomas@nocrew.org>
 *  Copyright 1999 Christer Gustavsson <cg@nocrew.org>
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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
/* sys/user.h needed for PAGE_SIZE */
#include <sys/user.h>
#include <linux/unistd.h>
#include <termios.h>

#include "div.h"
#include "version.h"
#include "prototypes.h"
#include "sysvars.h"
#include "traps.h"
#include "bios.h"
#include "xbios.h"
#include "gemdos.h"
#ifdef USE_XGEMDOS
#include "xgemdos.h"
#endif
#include "vt52.h"

static struct termios old_termios;
static int has_setup_tty = 0;

void setup_tty( void )
{
  struct termios flags;

  ioctl( 0, TCGETS, &flags );
  old_termios = flags;
  flags.c_iflag &= ~(INLCR | ICRNL);
  flags.c_lflag &= ~(ECHO | ECHOE | ECHOK | ICANON);
  flags.c_oflag &= ~ONLRET;
  ioctl( 0, TCSETS, &flags );
  has_setup_tty = 1;

  /*
   *  Set up everything that is needed for the vt52 emulator
   */
  setup_vt52();
  
#ifdef USE_XGEMDOS
  /* This will open the framebuffer by calling v_opnwk() */
  if( prog->gem )
    init_xgemdos();
#endif
}

void restore_tty( void )
{
#ifdef USE_XGEMDOS
  /* This will close the framebuffer by calling v_clswk() */
  if( prog->gem )
    exit_xgemdos();
#endif

  ioctl( 0, TCSETS, &old_termios );
  has_setup_tty = 0;
}

void switch_tty_mode( void )
{
  static struct termios switched_tty;
  static int is_switched = 0;

  if( !is_switched ) {
    ioctl( 0, TCGETS, &switched_tty );
    ioctl( 0, TCSETS, &old_termios );
    is_switched = 1;
  }
  else {
    ioctl( 0, TCSETS, &switched_tty );
    is_switched = 0;
  }
}

/* restore tty and exit */
void rexit( int code )
{
  if( has_setup_tty ) {
	restore_tty();
  }
  exit( code );
}
