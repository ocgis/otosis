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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <div.h>
#include <sysvars.h>
#include <prototypes.h>

extern TosProgram *prog;

void handle_trace( char *stack )
{
  char str[ 1024 ];
  char *command, *arg, *w;
  int end = 0;
  int c;
  Cookie *cookies;
  int nwords;
  int addr;
  TosBasepage *basepage;
  Dta *dta;
  int drive;

  switch_tty_mode();
  do {
    printf( "> " );
    fgets( str, 1022, stdin );
    if( str[ strlen( str ) - 1 ] == '\n' ) {
      str[ strlen( str ) - 1 ] = 0;
    }
    /*
     *  Split command into command and argument parts
     */
    for( command = str ; *command && isspace( *command ) ; command++ );
    for( arg = command ; *arg && !isspace( *arg ) ; arg++ );
    if( *arg ) {
      *arg++ = 0;
      while( *arg && isspace( *arg ) )  arg++;
      if( *arg ) {
	for( w = arg + strlen( arg ) - 1 ; w > arg && isspace( *w ) ; w-- );
	*(w + 1) = 0;
      }
    }
 
    if( strcmp( command, "step" ) == 0 || *command == 0 ) {
      end = 1;
    }
    else if( strcmp( command, "status" ) == 0 ) {
      printf( "MiNT emulation: %s\n", prog->emulate_mint ? "on" : "off" );
      printf( "User mode: %s\n", prog->super ? "supervisor" : "user" );
      printf( "TEXT segment: 0x%08x\n", (unsigned int)prog->text );
      printf( "DATA segment: 0x%08x\n", (unsigned int)prog->data );
      printf( "BSS segment: 0x%08x\n", (unsigned int)prog->bss );
      printf( "Basepage: 0x%08x\n", (unsigned int)prog->basepage );
      printf( "DTA: 0x%08x\n", (unsigned int)prog->dta );
      printf( "Allocated memory: 0x%08x\n", (unsigned int)prog->size );
      printf( "Current drive: %c\n", prog->curdrv + 'a' );
      printf( "MiNT domain: %d\n", prog->domain );
      printf( "User value: 0x%08x\n", (unsigned int)prog->usrval );
    }
    else if( strcmp( command, "stack" ) == 0 ) {
      nwords = 10;
      if( *arg ) {
	nwords = strtol( arg, NULL, 0 );
      }
      for( c = 0 ; c < nwords ; c++ ) {
	printf( "%2d: 0x%04x\n", c, *((unsigned short *)stack + c) );
      }
    }
    else if( strcmp( command, "string" ) == 0 ) {
      addr = strtol( arg, NULL, 0 );
      control_print( (char *)addr );
      printf( "\n" );
    }
    else if( strcmp( command, "dump" ) == 0 ) {
      addr = strtol( arg, NULL, 0 );
      for( c = 0 ; c < 16 * 10 ; c++ ) {
	if( c % 16 == 0 ) {
	  if( c != 0 ) {
	    printf( "\n" );
	  }
	  printf( "0x%08x:", addr + c );
	}
	printf( " %02x", *(unsigned char *)(addr + c) );
      }
      printf( "\n" );
    }
    else if( strcmp( command, "cookies" ) == 0 ) {
      for( cookies = (Cookie *)SYSVAR_COOKIES ; cookies->cookie ; cookies++ ) {
	printf( "%c%c%c%c: 0x%08x\n",
	       (int)(((ulong)cookies->cookie >> 24) & 0xFF),
	       (int)(((ulong)cookies->cookie >> 16) & 0xFF),
	       (int)(((ulong)cookies->cookie >> 8) & 0xFF),
	       (int)((ulong)cookies->cookie & 0xFF),
	       (int)((ulong)cookies->value) );
      }
    }
    else if( strcmp( command, "basepage" ) == 0 ) {
      basepage = prog->basepage;
      printf( "lowtpa: 0x%08lx\n", (ulong)basepage->lowtpa );
      printf( "hitpa: 0x%08lx\n", (ulong)basepage->hitpa );
      printf( "tbase: 0x%08lx\n", (ulong)basepage->tbase );
      printf( "tlen: 0x%08lx\n", basepage->tlen );
      printf( "dbase: 0x%08lx\n", (ulong)basepage->dbase );
      printf( "dlen: 0x%08lx\n", basepage->dlen );
      printf( "bbase: 0x%08lx\n", (ulong)basepage->bbase );
      printf( "blen: 0x%08lx\n", basepage->blen );
      printf( "dta: 0x%08lx\n", (ulong)basepage->dta );
      printf( "parent: 0x%08lx\n", (ulong)basepage->parent );
      printf( "env: 0x%08lx\n", (ulong)basepage->env );
      printf( "commandline: " );
      control_print( basepage->cmdlin );
      printf( "\n" );
    }
    else if( strcmp( command, "dta" ) == 0 ) {
      dta = prog->dta;
      printf( "attrib: %d\n", dta->d_attrib );
      printf( "time: %d\n", dta->d_time );
      printf( "date: %d\n", dta->d_date );
      printf( "length: %ld\n", dta->d_length );
      printf( "filename: %s\n", dta->d_fname );
    }
    else if( strcmp( command, "continue" ) == 0 ) {
      prog->trace = 0;
      end = 1;
    }
    else if( strcmp( command, "curdir" ) == 0 ) {
      drive = prog->curdrv;
      if( *arg ) {
	if( strlen( arg ) == 1 ) {
	  drive = tolower( *arg ) - 'a';
	}
      }
      printf( "current dir for drive %c: %s\n",
	     drive + 'a', prog->curdir[ drive ] );
    }
    else if( strcmp( command, "quit" ) == 0 ) {
      rexit( 0 );
    }
    else if( strcmp( command, "help" ) == 0 ) {
      printf( "Available commands:\n" );
      printf( "basepage - print the basepage\n" );
      printf( "dta      - print the DTA\n" );
      printf( "dump     - prints the memory at specified address\n" );
      printf( "continue - turn off trace mode and return to TOS program\n" );
      printf( "cookies  - print the cookie jar\n" );
      printf( "curdir   - print current directory for specified drive\n" );
      printf( "stack    - print a stack dump\n" );
      printf( "           optional argument: number of words to show\n" );
      printf( "status   - display information about the running program\n" );
      printf( "step     - return control to the running TOS program\n" );
      printf( "string   - Print a string at specified address\n" );
      printf( "quit     - terminate program\n" );
    }
    else {
      printf( "Illegal command\n" );
    }
  } while( !end );

  switch_tty_mode();
}

void control_print( char *str )
{
  while( *str ) {
    if( isprint( *str ) ) {
      putchar( *str );
    }
    else {
      printf( "\\%03o", *str );
    }
    str++;
  }
}

/* Local Variables:              */
/* tab-width: 8                  */
/* End:                          */
