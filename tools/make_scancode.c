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
#include <string.h>

int main( int argc, char **argv )
{
  FILE *fp;
  int ascii[ 256 ];
  char str[ 1024 ];
  int scancode;
  int line;
  char *token;
  int asciicode;
  char ascii_num[ 4 ];
  int c;
  int c2;

  if( argc != 2 ) {
    fprintf( stderr, "Usage: make_scancode filename\n" );
    exit( 1 );
  }

  if( (fp = fopen( argv[ 1 ], "r" )) == NULL ) {
    perror( "couldn't open input file" );
    exit( 1 );
  }

  for( c = 0 ; c < 256 ; c++ ) {
    ascii[ c ] = 0;
  }

  line = 0;
  while( fgets( str, 1024, fp ) != NULL ) {
    line++;
    if( str[ strlen( str ) - 1 ] == '\n' ) {
      str[ strlen( str ) - 1 ] = 0;
    }
    if( (token = strtok( str, "\t" )) == NULL ) {
      fprintf( stderr, "Syntax error in input file, line: %d\n", line );
      exit( 1 );
    }
    scancode = strtol( token, NULL, 0 );
    while( (token = strtok( NULL, "\t" )) != NULL ) {
      /*
       *  If the token starts and ends with singe quotes
       *  it is a literal character, subject for backslash parsing.
       *  If not, it is passed to strtol
       *
       */
      if( strlen( token ) >= 3 &&
	 token[ 0 ] == '\'' && token[ strlen( token ) - 1 ] == '\'' ) {
	if( token[ 1 ] == '\\' ) {
	  if( token[ 1 ] >= '0' && token[ 1 ] <= '9' ) {
	    ascii_num[ 0 ] = token[ 2 ];
	    ascii_num[ 1 ] = token[ 3 ];
	    ascii_num[ 2 ] = token[ 4 ];
	    ascii_num[ 3 ] = 0;
	    asciicode = strtol( ascii_num, NULL, 8 );
	  }
	  else {
	    switch( token[ 2 ] ) {
	    case 'n':
	      asciicode = '\n';
	      break;
	    case 'r':
	      asciicode = '\r';
	      break;
	    case 'e':
	      asciicode = '\e';
	      break;
	    case 't':
	      asciicode = '\t';
	      break;
	    default:
	      asciicode = token[ 2 ];
	    }
	  }
	}
	else {
	  asciicode = token[ 1 ];
	}
      }
      else {
	asciicode = strtol( token, NULL, 0 );
      }
      ascii[ asciicode ] = scancode;
    }
  }
  fclose( fp );

  printf( "int ascii_to_scancode[] = {\n" );
  for( c = 0 ; c < 256 / 8 ; c++ ) {
    printf( "\t" );
    for( c2 = 0 ; c2 < 8 ; c2++ ) {
      printf( "0x%02x, ", ascii[ c * 8 + c2 ] );
    }
    printf( "\n" );
  }
  printf( "};\n" );
  return 0;
}
