/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *  Copyright 1998 Tomas Berndtsson <tomas@nocrew.org>
 *  Copyright 1999 - 2001 Christer Gustavsson <cg@nocrew.org>
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

#include <netinet/in.h>
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
#include <asm/page.h>
#include <linux/user.h>
#include <linux/unistd.h>
#include <termios.h>

#include <libotosis.h>
#include <libotosis_option.h>
#include <libotosis_sysvars.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef USE_XGEMDOS
#include "xgemdos.h"
#endif
#include "debug.h"
#include "emulate.h"
#include "version.h"

/* Global variables */
extern TosProgram *prog;
extern volatile int in_emu;		 /* 1 if in emulator, 0 if running tos prog */
extern char *TosPrgName;			     /* basename of TOS program */
extern ulong *malloc_regions;    /* list of malloced memory regions */
extern int malloc_regions_size;	 /* size of malloc_regions */

#define DEFAULT_PRG	""			/* should be the GEM desktop later */


#ifdef mc68000
static void sigill_handler( int, int, struct sigcontext * );
static void setup_sysvars(void);
#endif /* mc68000 */
static void parse_cmdline_options( char *argv[], int start, int end );
static void setup_sig_handlers(void);
static void print_usage(void);

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
  if( prog->basepage->env == NULL )
  {
	prog->basepage->env = (char *)htonl((UInt32)mymalloc(2));
	w = 0;
	env_len = 2;
  }
  else
  {
    char * myenv;

    myenv = (char *)ntohl((UInt32)prog->basepage->env);

	for(w = 0 ; myenv[w] || myenv[w + 1] ; w++ );
	w++;
	env_len = w + 2;
  }
  env_len += 6 + strlen( TosPrgName ) + 2;
  prog->basepage->env =
    (char *)htonl((UInt32)myrealloc((char *)ntohl((UInt32)prog->basepage->env),
                            env_len));
  strcpy((char *)ntohl((UInt32)prog->basepage->env) + w, "ARGV=" );
  w += 6;
  strcpy((char *)ntohl((UInt32)prog->basepage->env) + w, TosPrgName );
  w += strlen( TosPrgName ) + 1;
  for( i = prgarg + 1 ; i < argc ; ++i ) {
	env_len += strlen( argv[ i ] ) + 1;
	prog->basepage->env =
      (char *)htonl((UInt32)myrealloc((char *)ntohl((UInt32)prog->basepage->env),
                                      env_len));
	strcpy((char *)ntohl((UInt32)prog->basepage->env) + w, argv[i]);
	w += strlen( argv[ i ] ) + 1;
  }
  ((char *)ntohl((UInt32)prog->basepage->env))[w] = 0;

  /* make std channels unbuffered */
  setvbuf( stdin, NULL, _IONBF, 0 );
  setvbuf( stdout, NULL, _IONBF, 0 );
  setvbuf( stderr, NULL, _IONBF, 0 );
  
  setup_drivemap();
  setup_tty();
#ifdef mc68000
  setup_sysvars();
#endif /* mc68000 */
  init_bios();
  init_gemdos();
  setup_sig_handlers();

  malloc_regions = (ulong *)mymalloc( sizeof( void * ) * 10 );
  malloc_regions_size = 10;
  *malloc_regions = 0;

  in_emu = 0;
  /* start up the TOS program; locate stack at top of TPA */
#ifdef mc68000
  __asm__ __volatile__
	  ( "movel %2,%/sp		\n\t"
	    "movel %0,%/sp@-	\n\t"
		"clrl  %/sp@-		\n\t"
		"subl  %/a0,%/a0	\n\t"
		"jmp   %1@"
		: /* no outputs */
		: "g" (prog->basepage), "a" (prog->text),
		  "g" (prog->basepage->hitpa) );
#else
  emulate(prog);
#endif
  
  return 0;
}


#ifdef mc68000
static
void sigill_handler( int sig, int vec, struct sigcontext *s )
{
  int insn, trap_num;

  /*
  ** FIXME
  ** sigcontext doesn't seem to be the same in ppc linux as in m68k linux
  */
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
#ifdef USE_XGEMDOS
		case 2:
          switch(s->sc_d0 & 0xffff)
          {
          case 115: /* VDI */
            vdi_call(s->sc_d1);
            break;
            
          case 200: /* AES */
            aes_call(s->sc_d1);
            break;
            
          default:
            fprintf(stderr, "Illegal Xgemdos call: 0x%x\n", CPUget_dreg(0));
          }
          break;
#endif
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
#endif /* mc68000 */

#ifdef mc68000
static
void
setup_sysvars( void )
{
  Cookie *cookies;
  int c = 0;

  /* Map the first page of memory for the system variables */
  mmap( 0, PAGE_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_FIXED | MAP_PRIVATE, 0, 0 );
  cookies = (Cookie *)mymalloc( sizeof( Cookie ) * 10 );
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
#endif /* mc68000 */

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

static
void sig_killed( int signum )
{
	fprintf( stderr, "Caught %s signal -- exiting\n", sys_siglist[signum] );
	rexit( 1 );
}


static
void sig_toggle_trace( int signum )
{
	prog->trace = !prog->trace;
}


static	
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

#ifdef mc68000
  SETSTACKSIG( SIGILL, sigill_handler, sigstack );
  SETSTACKSIG( SIGSEGV, sigsegv_handler, sigstack );
#endif /* mc68000 */

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


static
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
				if (!*q && op->type == OPTTYPE_BOOL) {					/* boolean options toggeled if empty value */
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






/* Local Variables: */
/* tab-width: 4     */
/* End:             */
