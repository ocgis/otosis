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

#ifndef _option_h
#define _option_h

typedef enum {
	OPTTYPE_INT, OPTTYPE_BOOL, OPTTYPE_STR
} OPTTYPE;

typedef union {
	int i;
	char *s;
} OPTVAL;

typedef struct {
	char	*name;
	OPTTYPE	type;
	char	letter;
	OPTVAL	value;
} OPTION;

extern OPTION Options[];

/* include defines for options */
#include "optiondef.h"

#endif  /* _option_h */


/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
