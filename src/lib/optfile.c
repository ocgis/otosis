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

/*
 * Syntax of configuration files:
 * ------------------------------
 *
 * There are two config files: a system-wide /etc/tos.conf, and user-specific
 * files $HOME/.tos. Settings in the latter override those from the global
 * file. User settings can be further overridden by command line options.
 *
 * Each file is seen as a series of lines, that can be:
 *   - a comment, starting with '#'
 *   - an assignment, of the form VARIABLE '=' VALUE
 *   - or a program section header, of the form PRGNAME ':'
 * Empty lines are ignored.
 *
 * Assignments in a program section are executed only if the name of the
 * current TOS program matches the name mentioned in the header. This way, you
 * can define variable values depending on the TOS program.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "div.h"
#include "prototypes.h"
#include "option.h"

char *TosPrgName = "";

/*
 * read all possible config files
 */
void read_configuration( void )

{	char	*home;
	
	read_config_file( OTOSIS_GLOBAL_CONFIG );
	if ((home = getenv( "HOME" ))) {
		char name[strlen(home)+strlen(OTOSIS_USER_CONFIG)+2];
		strcpy( name, home );
		strcat( name, "/" );
		strcat( name, OTOSIS_USER_CONFIG );
		read_config_file( name );
	}
}


/*
 * read a config file, if it exists
 */
int read_config_file( const char *filename )

{	FILE	*f;
	char	line[512], *p, *q;
	int		linenr;
	int		ignore = 0;
	
	if (!(f = fopen( filename, "r" ))) {
		/* don't complain about missing files... */
		if (errno != ENOENT)
			fprintf( stderr, "Cannot open config file %s: %s\n",
					 filename, strerror(errno) );
		return( -1 );
	}

	linenr = 0;
	while( fgets( line, sizeof(line), f ) ) {

		++linenr;
		/* strip whitespace at start and end, skip comments and empty lines */
		p = line + strspn( line, " \t" );
		if (!*p || *p == '#')
			continue;
		for( q = p+strlen(p)-1; q >= p && isspace(*q); --q )
			*q = 0;
		if (!*p)
			continue;

		/* look for start of prg section */
		if ((q = strchr( p, ':')) && q > p && !q[1]) {
			/* strip whitespace before ':' */
			while( q > p && isspace(q[-1]) ) --q;
			ignore = strncasecmp( TosPrgName, p, q-p ) != 0;
		}
		
		if (ignore)
			continue;
		if (!parse_config_entry( p ))
			fprintf( stderr, "Syntax error in %s line %d\n",
					 filename, linenr );
	}
	fclose( f );
	return( 0 );
}


int parse_config_entry( const char *line )

{	const char	*p, *strval;
	char		name[80], *q;
	OPTION		*op;
	
	/* get option name */
	for( p = line, q = name; *p && (isalnum(*p) || *p == '_' || *p == '-'); )
		*q++ = *p++;
	*q = 0;
	p += strspn( p, " \t" );
	if (*p != '=')
		return( 0 );

	/* skip '=' and following whitespace */
	++p;
	p += strspn( p, " \t" );

	strval = p;
	if (!*strval)
		return( 0 );

	for( op = Options; op->name; op++ )
		if (strcmp( name, op->name ) == 0) break;
	if (!op->name) {
		fprintf( stderr, "Unknown option %s\n", name );
		return( 0 );
	}
	return( set_option_val( op, strval, 1 ) );
}


int set_option_val( OPTION *op, const char *strval, int errmsg )

{	char	*p;
	
	switch( op->type ) {
	  case OPTTYPE_INT:
		op->value.i = strtol( strval, &p, 0 );
		if (p == strval) {
			if (errmsg)
				fprintf( stderr, "Number expected, found \"%s\"\n", strval );
			return( 0 );
		}
		DDEBUG( "setting option %s to %d\n", op->name, op->value.i );
		break;
		
	  case OPTTYPE_BOOL:
		if (strcasecmp( strval, "yes" ) == 0 ||
			strcasecmp( strval, "y" ) == 0 ||
			strcasecmp( strval, "on" ) == 0 ||
			strcasecmp( strval, "1" ) == 0)
			op->value.i = 1;
		else if (strcasecmp( strval, "no" ) == 0 ||
			strcasecmp( strval, "n" ) == 0 ||
			strcasecmp( strval, "off" ) == 0 ||
			strcasecmp( strval, "0" ) == 0)
			op->value.i = 0;
		else {
			if (errmsg)
				fprintf( stderr, "Boolean expected, found \"%s\"\n", strval );
			return( 0 );
		}
		DDEBUG( "setting option %s to %d\n", op->name, op->value.i );
		break;
		
	  case OPTTYPE_STR:
		op->value.s = mystrdup(strval);
		DDEBUG( "setting option %s to \"%s\"\n", op->name, op->value.s );
		break;
		
	  default:
		if (errmsg)
			fprintf( stderr, "Internal error: bad option type %d\n",
					 op->type );
		return( 0 );
	}
	return( 1 );
}


/* Local Variables:              */
/* tab-width: 4                  */
/* End:                          */
