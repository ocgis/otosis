/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
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
#include <compile.h>
#include <version.h>


void print_version( void )
{
  fprintf( stderr, "oTOSis version " OTOSIS_VERSION "\n" );
  fprintf( stderr, "Compiled " OTOSIS_COMPILE_TIME " by "
		   OTOSIS_COMPILE_BY " on " OTOSIS_COMPILE_HOST "\n" );
  fprintf( stderr, "with " OTOSIS_COMPILER "\n\n" );
  fprintf( stderr, "oTOSis comes with ABSOLUTELY NO WARRANTY\n" );
  fprintf( stderr, "This is free software, and you are welcome to redistribute it\n" );
  fprintf( stderr, "under certain conditions; see the file COPYING for details.\n" );

  exit( 0 );
}

