/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *   *  Copyright 1999 Christer Gustavsson <cg@nocrew.org>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "div.h"
#include "init.h"
#include "prototypes.h"

extern TosProgram * prog;

#define TRUE 1

/*
** Exported
**
** 1999-01-16 CG
** 1999-01-25 CG
** 1999-02-02 CG
*/
TosProgram *
new_program (void) {
  TosProgram * nprog = mymalloc (sizeof (TosProgram));

  /* FIXME */
  prog = nprog;

  read_configuration ();

  setup_drivemap ();

  /* Default for the lib is to emulate mint calls */
  nprog->emulate_mint = TRUE;

  return nprog;
}


#define SETUP_MAP(CHR,NUM) \
  setup_map( Opt_drive ## CHR, Opt_current ## CHR, NUM );

static void setup_map( char *map, char *curr, int num )
{
  char *newcurr = curr;
  char currdir[ 1024 ];
  char currdir_tos[ 1024 ];

  if( strcmp( map, "-" ) == 0 ) {
	prog->drive_map[ num ] = NULL;
	prog->curdir[ num ] = NULL;
  }
  else {
	if( strcmp( curr, "-" ) == 0 ) {
	  if( strcmp( map, "/" ) != 0 ) {
		printf( "Error setting current%c\n"
			   "Auto setting of current directory requires drive map "
			   "to map to /.\n", num + 'A' );
		newcurr = "\\";
	  }
	  if( getcwd( currdir, 1024 ) == NULL ) {
		perror( "error getting current directory" );
		rexit( 1 );
	  }
	  if( Opt_MiNT && Opt_auto_current_long ) {
		unix_to_tos_long( currdir_tos, currdir );
	  }
	  else {
		unix_to_tos_path_short( currdir_tos, currdir );
	  }
	  newcurr = currdir_tos;
	}
	else if( *curr != '\\' ) {
	  printf( "curr = %s\n", curr );
	  printf( "Illegal current directory option: current%c. "
			 "Must be absolute.\n", num + 'A' );
	  newcurr = "\\";
	}
	DDEBUG( "mapping root directory of drive %c to: %s\n",
		   num + 'a', map );
	DDEBUG( "mapping current directory of drive %c to: %s\n",
		   num + 'a', newcurr );
	prog->drive_map[ num ] = mymalloc( 1024 );
	strcpy( prog->drive_map[ num ], map );
	prog->curdir[ num ] = mymalloc( 1024 );
	strcpy( prog->curdir[ num ], newcurr );
  }
}


void setup_drivemap( void )
{
  int drvchar;
  char *currpath;

  SETUP_MAP(A,0);
  SETUP_MAP(B,1);
  SETUP_MAP(C,2);
  SETUP_MAP(D,3);
  SETUP_MAP(E,4);
  SETUP_MAP(F,5);
  SETUP_MAP(G,6);
  SETUP_MAP(H,7);
  SETUP_MAP(I,8);
  SETUP_MAP(J,9);
  SETUP_MAP(K,10);
  SETUP_MAP(L,11);
  SETUP_MAP(M,12);
  SETUP_MAP(N,13);
  SETUP_MAP(O,14);
  SETUP_MAP(P,15);
  SETUP_MAP(Q,16);
  SETUP_MAP(R,17);
  SETUP_MAP(S,18);
  SETUP_MAP(T,19);
  SETUP_MAP(V,21);
  SETUP_MAP(W,22);
  SETUP_MAP(X,23);
  SETUP_MAP(Y,24);
  SETUP_MAP(Z,25);

  prog->drive_map[ 'u' - 'a' ] = NULL;

  currpath = Opt_current_drive;
  drvchar = currpath[ 0 ];
  if( isupper( drvchar ) ) {
	drvchar = tolower( drvchar );
  }
  if( drvchar < 'a' || drvchar > 'z' ) {
	DDEBUG( "Illegal current drive specification: '%s'\n", Opt_current_drive );
	drvchar = 'c';
  }
  prog->curdrv = drvchar - 'a';
}
