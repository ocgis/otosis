/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
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

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

/* main */
/*void sigill_handler( int, int, struct sigcontext_struct * );*/
/* defined in tos.c */
void setup_sysvars( void );
void setup_drivemap( void );
void print_usage( void );
__NORETURN void rexit( int code ) __NORETURN2;

/* trap handler */
/*void sigsegv_handler( int, int, struct sigcontext_struct * );*/
void setup_sig_handlers( void );
void bombs( int );

/* version.c */
void print_version( void );

/* hardware register emulator */
int dispatch_hwreg_emu( ulong, int, int, ulong * );

/* exec loader */
TosProgram *load_tos_program( char * );

/* terminal functions */
void setup_tty( void );
void restore_tty( void );
void switch_tty_mode( void );

/* memory functions */
void *mymalloc( size_t );
void *myrealloc( void *, size_t );
char *mystrdup( char * );

/* functions for the file handling emulation */
int translate_error( int );

int tos_to_unix( char *, char * );
void unix_to_tos_path( char *, char * );
void unix_to_tos_file( char *, char * );

void tos_to_unix_short( char *, char * );
void unix_to_tos_path_short( char *, char * );
void unix_to_tos_file_short( char *, char * );

void tos_to_unix_long( char *, char * );
void unix_to_tos_long( char *, char * );

void get_basename( char *, char * );
void get_dirname( char *, char * );
void unix_time_to_tos( short *, short *, time_t );

int resolve_tos_file( char *, char * );

/* optfile.c */
void read_configuration( void );
int read_config_file( const char *filename );
int parse_config_entry( const char *line );
int set_option_val( OPTION *op, const char *strval, int errmsg );

/* strace.c */
void strace_begin( const char *call_name, const char *call_format,
		   const char *argp );
void strace_end( const char *call_name, const char *rv_format,
		 const char *argp, long rv );

/* trace.c */
void handle_trace( char * );
void control_print( char * );

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
