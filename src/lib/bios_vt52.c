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

#include <unistd.h>
#include <sys/types.h>
#include <ncurses/curses.h>
#include <ncurses/term.h>
#include <sys/ioctl.h>

#include "div.h"
#include "vt52.h"


typedef struct {
  int term_lines;
  int term_cols;
  char *cuu1;
  char *cud1;
  char *cuf1;
  char *cub1;
  char *clear;
  char *home;
  char *ed;
  char *el;
  char *il1;
  char *dl1;
  char *civis;
  char *cnorm;
  char *sc;
  char *rc;
  char *rev;
  char *sgr0;
  char *cup;
} TTYInfo;


#ifdef TEST
void setup_tty( void );
void restore_tty( void );
#endif

static int my_putchar( int );

#ifdef TEST
struct termios old_termios;
#endif

TTYInfo tty_info;


#ifdef TEST
int main( int argc, char **argv )
{
  char *ptr;

  setup_tty();

#if 0
  tputs( tigetstr( "home" ), 1, putchar );
  printf( "Blah!" );
  tputs( tparm( tigetstr( "cup" ), 5, 10 ), 1, putchar );
  printf( "Moving!" );
  tputs( tigetstr( "rev" ), 1, putchar );
  printf( "fan" );
#endif

#if 0
  vt52_put_string( "hejsan\eAhejhopp\eAhej igen\ephej\eqblabb" );
  vt52_put_string( "\eY\044\106abcedfgh\eHDetta ar homepositionen\n" );
#endif

  if( argc != 2 ) {
    printf( "you need an argument\n" );
  }
  else {
    for( ptr = argv[ 1 ] ; *ptr ; *ptr++ ) {
      if( *ptr == '$' )  *ptr = '\e';
    }
    vt52_put_string( argv[ 1 ], 0 );
  }

  restore_tty();
}


#endif


static int vt52_fd;

static int my_putchar( int xc )
{
    unsigned char c = xc;
    return write( vt52_fd, &c, 1 );
}


void vt52_put_char( int ch, int fd )
{
  static int escape_mode = 0;
  static char buf[ 4 ];
  static char *buf_ptr;
  static int left = 0;
  char *tty_out;
  int new_x;
  int new_y;

  vt52_fd = fd;
  
  /*  There are three escape sequences that work
   *  differently from the normal ones in that they
   *  take one or more arguments. These are:
   *  'Y': Posision cursor. Two arguments, X and Y (with 32 added)
   *  'b': Set foreground colour. One argument.
   *  'c': Set background colour. One argument.
   *
   *  The variable escape_mode specifies in which "mode"
   *  the emulator is in:
   *  0 = Normal mode
   *  1 = Escape mode (esc was the last character to have been receieved)
   *  2 = Position cursor mode
   *  3 = Set foreground mode
   *  4 = Set background mode
   *
   *  The variable left specifies how characters are left
   *  to receieve before the handler is called.
   *
   *   /elias
   */

  if( escape_mode == 0 ) {
    /* normal mode */
    if( ch == '\e' ) {
      escape_mode = 1;
    }
    else {
      my_putchar( ch );
    }
    return;
  }

  /* if left is not 0 then write the character to the buffer */
  if( left ) {
    *buf_ptr++ = ch;
    if( --left ) {
      return;
    }
  }

  if( escape_mode == 1 ) {
    /*
     *  In an escape sequence
     *  handle all "simple" cases (no arguments)
     */
    switch( ch ) {
    case 'A':			/* cursor up one line*/
      tty_out = tty_info.cuu1;
      break;
    case 'B':			/* cursor down one line */
      tty_out = tty_info.cud1;
      break;
    case 'C':			/* cursor right one column */
      tty_out = tty_info.cuf1;
      break;
    case 'D':			/* cursor left one column */
      tty_out = tty_info.cub1;
      break;
    case 'E':			/* clear screen and move cursor to home */
      tty_out = tty_info.clear;
      break;
    case 'H':			/* move cursor to home */
      tty_out = tty_info.home;
      break;
    case 'I':			/* cursor up and scroll screen */
      /* not implemented */
      tty_out = "";
      break;
    case 'J':			/* erase the screen down from cursor pos */
      tty_out = tty_info.ed;
      break;
    case 'K':			/* clear to end of line */
      tty_out = tty_info.el;
      break;
    case 'L':			/* insert line at cursor position */
      tty_out = tty_info.il1;
      break;
    case 'M':			/* delete line and scroll up */
      tty_out = tty_info.dl1;
      break;
    case 'Y':			/* position cursor */
      escape_mode = 2;
      buf_ptr = buf;
      left = 2;
      return;
    case 'b':			/* set foregrund colour */
      escape_mode = 3;
      buf_ptr = buf;
      left = 1;
      return;
    case 'c':			/* set background colour */
      escape_mode = 4;
      buf_ptr = buf;
      left = 1;
      return;
    case 'd':			/* erase the screen to cursor position */
      /* not implemented */
      tty_out = "";
      break;
    case 'e':			/* disable the cursor */
      tty_out = tty_info.civis;
      break;
    case 'f':			/* enable the cursor */
      tty_out = tty_info.cnorm;
      break;
    case 'j':			/* save cursor position */
      tty_out = tty_info.sc;
      break;
    case 'k':			/* restore cursor position */
      tty_out = tty_info.rc;
      break;
    case 'l':			/* erase current line and place cursor */
				/* at the leftmost column */
      /* not implemented */
      break;
    case 'o':			/* erase line from start of line */
				/* to the cursor position */
      /* not implemented */
      tty_out = "";
      break;
    case 'p':			/* enable reverse mode */
      tty_out = tty_info.rev;
      break;
    case 'q':			/* disable reverse emode */
      tty_out = tty_info.sgr0;
      break;
    case 'v':			/* enable line wrap */
      /* not implemented */
      tty_out = "";
      break;
    case 'w':			/* disable line wrap */
      /* not implemented */
      tty_out = "";
      break;
    default:
      tty_out = "";
    }
    tputs( tty_out, 1, my_putchar );
    escape_mode = 0;
    return;
  }

  if( escape_mode == 2 ) {
    /*  In set cursor position mode
     *  the X and Y coordinates has
     *  32 added to them
     */
    new_x = buf[ 1 ] < 32 ? 0 : buf[ 1 ] - 32;
    new_y = buf[ 0 ] < 32 ? 0 : buf[ 0 ] - 32;
    tputs( tparm( tty_info.cup, new_y, new_x ), 1, my_putchar );
    escape_mode = 0;
    return;
  }

  if( escape_mode == 3 || escape_mode == 4 ) {
    /* colour not implemented */
    escape_mode = 0;
  }
    
}

void vt52_put_string( char *str, int fd )
{
  char *ptr = str;
  while( *ptr ) {
    vt52_put_char( *ptr++, fd );
  }
}


#ifdef TEST

void setup_tty( void )
{
  struct termios flags;

  ioctl( 0, TCGETS, &flags );
  old_termios = flags;
  flags.c_iflag &= ~(INLCR | ICRNL);
  flags.c_lflag &= ~(ECHO | ECHOE | ECHOK | ICANON);
  flags.c_oflag &= ~(ONLRET | ONLCR);
  ioctl( 0, TCSETS, &flags );

  setup_vt52();
}

void restore_tty( void )
{
  ioctl( 0, TCSETS, &old_termios );
}

#endif

void setup_vt52( void )
{
  /*
   *  I don't have a manual available for TIOCGWINSZ
   *  therefore these are hardcoded    /elias
   */
  tty_info.term_lines = 25;
  tty_info.term_cols = 80;
  setupterm( NULL, 1, NULL );

  tty_info.cuu1 = tigetstr( "cuu1" );
  tty_info.cud1 = tigetstr( "cud1" );
  tty_info.cuf1 = tigetstr( "cuf1" );
  tty_info.cub1 = tigetstr( "cub1" );
  tty_info.clear = tigetstr( "clear" );
  tty_info.home = tigetstr( "home" );
  tty_info.ed = tigetstr( "ed" );
  tty_info.el = tigetstr( "el" );
  tty_info.il1 = tigetstr( "il1" );
  tty_info.dl1 = tigetstr( "dl1" );
  tty_info.civis = tigetstr( "civis" );
  tty_info.cnorm = tigetstr( "cnorm" );
  tty_info.sc = tigetstr( "sc" );
  tty_info.rc = tigetstr( "rc" );
  tty_info.rev = tigetstr( "rev" );
  tty_info.sgr0 = tigetstr( "sgr0" );
  tty_info.cup = tigetstr( "cup" );
}

/* Local Variables:              */
/* tab-width: 8                  */
/* End:                          */
