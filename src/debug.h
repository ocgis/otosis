/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
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

#ifdef DEBUG

extern int mid_of_line;		/* declared in strace.c */

#define CHECK_MID_OF_LINE()			\
    do {					\
	if (mid_of_line) {			\
	    fprintf( stderr, "\n" );		\
	    mid_of_line = 0;			\
	}					\
    } while(0)

#define DDEBUG(fmt,args...)			\
    do {					\
	if (Opt_debug) {			\
	    CHECK_MID_OF_LINE();		\
	    fprintf( stderr, fmt, ## args);	\
	}					\
    } while(0)

#define STRACE_BEGIN(mod,a)				\
    do {						\
	if (Opt_debug)					\
	    strace_begin( mod##_call_names[callnum],	\
			  mod##_call_args[callnum],	\
			  a );				\
	if (prog->trace) {				\
	    CHECK_MID_OF_LINE();			\
	    handle_trace( a );				\
	}						\
    } while(0)

#define STRACE_END(mod,a,r)				\
    do {						\
    	if (Opt_debug)					\
	    strace_end( mod##_call_names[callnum],	\
			mod##_call_retv[callnum],	\
			a, r );				\
    } while(0)

#else
#define DDEBUG(fmt,args...)
#define	STRACE_BEGIN(mod,a)
#define	STRACE_END(mod,a,r)
#endif
