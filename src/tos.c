/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *  Copyright 1998 Tomas Berndtsson <tomas@nocrew.org>
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
#include <termbits.h>

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
#endif /* USE_XGEMDOS */
#include "option.h"
#include "vt52.h"

/* Global variables */
TosProgram *prog;
int in_emu = 1;					/* 1 if in emulator, 0 if running tos prog */
char *TosPrgName;				/* basename of TOS program */
ulong *malloc_regions;			/* list of malloced memory regions */
int malloc_regions_size;		/* size of malloc_regions */

static struct termios old_termios;
static int has_setup_tty = 0;

#define DEFAULT_PRG	""			/* should be the GEM desktop later */


void sigill_handler( int, int, struct sigcontext * );
static void parse_cmdline_options( char *argv[], int start, int end );

int main( int argc, char **argv )
{
  char *cmdlin, *p;
  int i, prgarg;
  int w;
  int env_len;

  /* pre-parse the arguments to scan for program to start */
  for( prgarg = 1; prgarg < argc && argv[prgarg][0] == '-'; ++prgarg ) {
	  if (strcmp( argv[prgarg], "--" ) == 0) {
		  ++prgarg;
		  break;
	  }
  }

  /* if no prg name on cmd line, use default prg */
  TosPrgName = (prgarg < argc) ? argv[prgarg] : DEFAULT_PRG;
  if ((p = strrchr( TosPrgName, '/' )) || (p = strrchr( TosPrgName, '\\' )))
	  TosPrgName = p+1;

  /* read config files */
  read_configuration();
  /* now process cmd line options, they override the config files */
  if (prgarg > 1)
	  parse_cmdline_options( argv, 1, prgarg-1 );
  
  if (!TosPrgName || !*TosPrgName) {
	  fprintf( stderr, "No program to start!\n" );
	  print_usage();
  }
  DDEBUG( "Loading TOS program: %s\n", argv[prgarg] );
  if (!(prog = load_tos_program( argv[prgarg] )))
	  exit( 1 );

  /*
   *  Set up the normal commandline
   */
  cmdlin = prog->basepage->cmdlin + 1;
  *cmdlin = 0;
  for( i = prgarg+1; i < argc ; ++i ) {
	  if (strlen(cmdlin) + strlen(argv[i]) + 2 > 124) {
		  DDEBUG( "Command line too long\n" );
		  break;
	  }
	  strcat( cmdlin, argv[i] );
	  if (i != argc-1) strcat( cmdlin, " " );
  }
/*  prog->basepage->cmdlin[0] = (unsigned char)strlen( cmdlin );*/
  prog->basepage->cmdlin[0] = 127;
  /*
   *  Set up the ARGV commandline
   */
  if( prog->basepage->env == NULL ) {
	prog->basepage->env = mymalloc( 2 );
	w = 0;
	env_len = 2;
  }
  else {
	for( w = 0 ; prog->basepage->env[ w ] ||
		         prog->basepage->env[ w + 1 ] ; w++ );
	w++;
	env_len = w + 2;
  }
  env_len += 6 + strlen( TosPrgName ) + 2;
  prog->basepage->env = myrealloc( prog->basepage->env, env_len );
  strcpy( prog->basepage->env + w, "ARGV=" );
  w += 6;
  strcpy( prog->basepage->env + w, TosPrgName );
  w += strlen( TosPrgName ) + 1;
  for( i = prgarg + 1 ; i < argc ; ++i ) {
	env_len += strlen( argv[ i ] ) + 1;
	prog->basepage->env = myrealloc( prog->basepage->env, env_len );
	strcpy( prog->basepage->env + w, argv[ i ] );
	w += strlen( argv[ i ] ) + 1;
  }
  prog->basepage->env[ w ] = 0;
	


  /* make std channels unbuffered */
  setvbuf( stdin, NULL, _IONBF, 0 );
  setvbuf( stdout, NULL, _IONBF, 0 );
  setvbuf( stderr, NULL, _IONBF, 0 );
  
  setup_drivemap();
  setup_tty();
  setup_sysvars();
  init_bios();
  init_gemdos();
  setup_sig_handlers();

  malloc_regions = mymalloc( sizeof( void * ) * 10 );
  malloc_regions_size = 10;
  *malloc_regions = 0;

  in_emu = 0;
  /* start up the TOS program; locate stack at top of TPA */
  __asm__ __volatile__
	  ( "movel %2,%/sp		\n\t"
	    "movel %0,%/sp@-	\n\t"
		"clrl  %/sp@-		\n\t"
		"subl  %/a0,%/a0	\n\t"
		"jmp   %1@"
		: /* no outputs */
		: "g" (prog->basepage), "a" (prog->text),
		  "g" (prog->basepage->hitpa) );

  return 0;
}

void sigill_handler( int sig, int vec, struct sigcontext *s )
{
  int insn, trap_num;
  
  if( in_emu ) {
    printf( "SIGILL in emulator code (pc=%08lx insn=%04x).\n",
			s->sc_pc, *(ushort *)(s->sc_pc) );
    rexit( 1 );
  }
  in_emu = 1;

  switch( vec>>2 ) {

	case VEC_PRIV:
	  /* stacked PC points to offending instruction */
	  insn = *(ushort *)(s->sc_pc);

	  /* Check for MOVE SR,.. instruction used by some 68000 programs, and
	   * change it into MOVE CCR,.. */
	  if ((insn & 0xffc0) == 0x40c0) {
		  *(ushort *)(s->sc_pc) |= 0x0200;
		  DDEBUG( "Changed MOVE SR,.. insn at %08lx to MOVE CCR,..\n",
				  s->sc_pc );
		  /* just returning re-runs the insn */
		  break;
	  }
	  else
		  goto do_bombs;

	case VEC_TRAP1 ... VEC_TRAP15:
	  /* stacked PC points after TRAP insn */
	  insn = *(ushort *)(s->sc_pc - 2);
	  if( (insn & 0xFFF0) != 0x4E40 ){
		  printf( "SIGILL not called by trap insn, code = 0x%04x\n", insn );
	  }
	  trap_num = insn & 0xF;

	  switch( trap_num ){
		case 1:
		  s->sc_d0 = dispatch_gemdos( (char *)s->sc_usp );
		  break;
		case 2:
#ifdef USE_XGEMDOS
		  /* xgemdos is special case, arguments aren't sent through stack
		     This method will do for now */
		  dispatch_xgemdos( s->sc_d0, s->sc_d1 );
#else
		  printf( "oVDIsis not yet implemented. Complain to:\n"
				  "Tomas Berndtsson (tomas@nocrew.org)\n" );
#endif
		  break;
		case 13:
		  s->sc_d0 = dispatch_bios( (char *)s->sc_usp );
		  break;
		case 14:
		  s->sc_d0 = dispatch_xbios( (char *)s->sc_usp );
		  break;
		default:
		  printf( "cannot handle TRAP #%d insn\n", trap_num );
		  bombs( 32 + trap_num );
	  }
	  /* just returning continues with next insn */
	  break;

	default:
	do_bombs:
	  DDEBUG( "Unhandable SIGILL (exception %d at pc=%08lx)\n",
			  vec, s->sc_pc );
	  bombs( vec );
	  
  }

  in_emu = 0;
}

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
#endif /* USE_XGEMDOS */
}

void restore_tty( void )
{
#ifdef USE_XGEMDOS
  /* This will close the framebuffer by calling v_clswk() */
  if( prog->gem )
    exit_xgemdos();
#endif /* USE_XGEMDOS */

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

void setup_sysvars( void )
{
  Cookie *cookies;
  int c = 0;

/* Map the first page of memory for the system variables */
  mmap( 0, PAGE_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_FIXED | MAP_PRIVATE, 0, 0 );
  cookies = mymalloc( sizeof( Cookie ) * 10 );
  cookies[ c ].cookie = 0x4F544F53; /* OTOS */
  cookies[ c++ ].value = (OTOSIS_VERSION_MAJOR << 8) | OTOSIS_VERSION_MINOR;
  cookies[ c ].cookie = 0x5F435055; /* _CPU */
  cookies[ c++ ].value = 30;
  if( prog->emulate_mint ) {
    cookies[ c ].cookie = 0x4D694E54; /* MiNT */
    cookies[ c++ ].value = 0x010C;
  }
  cookies[ c ].cookie = 0;
  cookies[ c++ ].value = 9 - c;
  SYSVAR_COOKIES = (ulong)cookies;
}

/*
 * The current libc unfortunately still sets sa_restorer in sigaction(),
 * though that fields is unused since ages. The workaround to get our
 * sa_restorer setting through to the kernel, is to define an own direct
 * syscall 'pure_sigaction', i.e. not calling the libc function.
 */

#define __NR_pure_sigaction	__NR_sigaction

static inline _syscall3(int,pure_sigaction,
						int,sig,
						struct sigaction *,act,
						struct sigaction *,oldact)


#define SETSIG(num,handler)	({						\
				    sigset_t mask;				\
				    struct sigaction sa;			\
				    sigemptyset(&mask);				\
				    sa.sa_mask = mask;				\
				    sa.sa_handler = (__sighandler_t)handler;	\
				    sa.sa_flags = SA_RESTART;			\
				    sigaction( num, &sa, NULL );		\
				  })

#define SETSTACKSIG(num,handler,stack)	({					\
				    sigset_t mask;				\
				    struct sigaction sa;			\
				    sigemptyset(&mask);				\
				    sa.sa_mask = mask;				\
				    sa.sa_handler = (__sighandler_t)handler;	\
				    sa.sa_restorer = (void (*)(void))stack;	\
				    sa.sa_flags = SA_NOMASK | SA_STACK;		\
				    pure_sigaction( num, &sa, NULL );		\
				  })

extern void sigsegv_handler( int, int, struct sigcontext * );

void sig_killed( int signum )
{
	fprintf( stderr, "Caught %s signal -- exiting\n", sys_siglist[signum] );
	rexit( 1 );
}

void sig_toggle_trace( int signum )
{
	prog->trace = !prog->trace;
}
	
void setup_sig_handlers( void )
{
  unsigned long sigstack;
	
  /* The stack for the (non-exiting) signals SIGILL and SIGSEGV is located in
   * the normal Unix stack area (just below TASK_SIZE). This is in opposite to
   * the TOS stack, which is inside the TPA of the TOS program. This stack
   * switch avoids TOS stack overflows, since all the data pushed by the
   * signal handling in the kernel, and by the handler are often too much for
   * the small TOS stacks. Furthermore, the standard Unix stack is already a
   * self-extending VM_GROWSDOWN area, which is not easy to setup manually.
   *
   * For determining the current stack, the address of a local variable is
   * used. The offset 128 is for other data that may be pushed on the stack in
   * this function. (Also, the kernel doesn't allow accesses more than 256
   * bytes below USP...) Because of this method, it is important that the main
   * stack is switched to the TOS stack (in the TPA) immediately after this
   * function returns!
   *
   * Using a different stack for (some) signals requires that the kernel
   * implements the SA_STACK flag for sigaction. But it's no harm if it
   * doesn't, the old (TOS) stack will be used for signals, too. Just could
   * happen that the TOS stack overflows...
   */

  sigstack = (unsigned long)&sigstack - 128;

  SETSTACKSIG( SIGILL, sigill_handler, sigstack );
  SETSTACKSIG( SIGSEGV, sigsegv_handler, sigstack );

  SETSIG( SIGHUP,    sig_killed );
  SETSIG( SIGINT,    sig_killed );
  SETSIG( SIGQUIT,   sig_killed );
  SETSIG( SIGTRAP,   sig_killed );
  SETSIG( SIGABRT,   sig_killed );
  SETSIG( SIGIOT,    sig_killed );
  SETSIG( SIGBUS,    sig_killed );
  SETSIG( SIGFPE,    sig_killed );
  SETSIG( SIGUSR1,   sig_toggle_trace );
  SETSIG( SIGUSR2,   sig_killed );
  SETSIG( SIGPIPE,   sig_killed );
  SETSIG( SIGALRM,   sig_killed );
  SETSIG( SIGTERM,   sig_killed );
  SETSIG( SIGSTKFLT, sig_killed );
  SETSIG( SIGURG,    sig_killed );
  SETSIG( SIGXCPU,   sig_killed );
  SETSIG( SIGXFSZ,   sig_killed );
  SETSIG( SIGVTALRM, sig_killed );
  SETSIG( SIGPROF,   sig_killed );
  SETSIG( SIGIO,     sig_killed );
  SETSIG( SIGPWR,    sig_killed );
}



void print_usage( void )
{
  printf( "Usage: tos -[dhvm] [-r extra_memory] tos_program "
	"[tos_program_args]\n" );
  exit( 0 );
}


/*
 * Parse options on the command line
 *
 * Possible one-letter options are as defined in the 3rd field of options.def.
 * Additional (and reserved) ones:
 *  -h: help
 *  -v: print version
 *  -o: set long name option (1st column of options.def)
 */

static void parse_cmdline_options( char *argv[], int start, int end )

{	int		i;
	char	*p, *q;
	OPTION	*op;
	
	for( i = start; i <= end; ++i ) {
		if (argv[i][0] != '-')
			/* shouldn't happen... */
			continue;
		if (argv[i][1] == '-' && argv[i][2] == 0)
			/* "--" arg ends options */
			return;
		
		for( p = argv[i] + 1; *p; ++p ) {
			if (*p == 'h') {
				print_usage();
				exit( 0 );
			}
			else if (*p == 'v') {
				print_version();
				exit( 0 );
			}
			else if (*p == '-') {
				++p;
				if ((q = strchr( p, '=' )))
					*q++ = 0;
				for( op = Options; op->name; ++op )
					if (strcmp( op->name, p ) == 0 ||
						(strncmp( p, "no", 2 ) == 0 &&
						 strcmp( op->name, p+2 ) == 0))
						break;
				if (!op->name) {
					fprintf( stderr, "Unknown option '%s'\n", p );
					exit( 1 );
				}
				if (!q && op->type != OPTTYPE_BOOL) {
					fprintf( stderr, "Option %s missing value\n", p );
					exit( 1 );
				}
				if (strncmp( p, "no", 2 )==0 && strcmp( op->name, p+2 )==0 ) {
					if (op->type != OPTTYPE_BOOL) {
						fprintf( stderr, "Option 'no' variation %s for "
								 "non-boolean value\n", p );
						exit( 1 );
					}
					if (q) {
						fprintf( stderr, "Option 'no' variation %s together "
								 "with value\n", p );
						exit( 1 );
					}
				}
				if (op->type == OPTTYPE_BOOL && !q)
					q = (strncmp( p, "no", 2 ) == 0) ? "no" : "yes";
				goto set_val;
			}
			else {
				for( op = Options; op->name; ++op )
					if (op->letter == *p) break;
				if (!op->name) {
					fprintf( stderr, "Unknown option '%c'\n", *p );
					exit( 1 );
				}
				/* boolean single-letter options have no argument */
				q = (op->type == OPTTYPE_BOOL) ? "" : p+1;
			  set_val:
				/* set option 'op' to value in 'p' */
				if (!*q && op->type == OPTTYPE_BOOL) {
					/* boolean options toggeled if empty value */
					op->value.i = !op->value.i;
				}
				else if (set_option_val( op, q, 1 ))
					/* leave inner loop and continue with next arg */
					break;
				else {
					fprintf( stderr, "Bad value for option %c\n", op->letter );
					exit( 1 );
				}
			}
		}
	}
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




/* Local Variables: */
/* tab-width: 4     */
/* End:             */
