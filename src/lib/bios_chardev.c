/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
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
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "div.h"
#include "prototypes.h"
#include "bios.h"
#include "gemdos.h"
#include "toserrors.h"

#include "vt52.h"

#define	BIOS_DEV_PRT	0
#define	BIOS_DEV_AUX	1
#define	BIOS_DEV_CON	2
#define	BIOS_DEV_MIDI	3
#define	BIOS_DEV_IKBD	4
#define	BIOS_DEV_RAWCON	5
#define	BIOS_DEV_MOD1	6
#define	BIOS_DEV_MOD2	7
#define	BIOS_DEV_SER1	8
#define	BIOS_DEV_SER2	9

#define MIN_BIOS_DEV 	BIOS_DEV_PRT
#define MAX_BIOS_DEV 	BIOS_DEV_SER2
#define N_BIOS_DEVS 	(MAX_BIOS_DEV+1)

#define	BIOS_DEV_CONIN	BIOS_DEV_CON
#define	BIOS_DEV_CONOUT	BIOS_DEV_RAWCON

#define	NOT_OPEN		-1

int BiosDevFd[N_BIOS_DEVS];

/* Bconmap() value */
int CurrentSerial = BIOS_DEV_MOD1;

/* Global variable for lpr pipe */
FILE *LprPipe = NULL;

/* Constant global sigset with SIGALRM */
sigset_t ALRM_sigset;

/* ASCII to scancode conversion table */
extern int ascii_to_scancode[];

/***************************** Prototypes *****************************/

static long emulate_scancode( unsigned char c );
static void start_lpr_timeout( void );
static void lpr_times_out( void );
static int get_biosdev_fd( int bdev, int i_o );

/************************* End of Prototypes **************************/



/*
 * The following functions are wrappers, calling a "normal" function with
 * "norrmal" argument layout (not via _args). This make it possible to also
 * call the BIOS functions from inside the emulator.
 */

BIOSFUNC(Bconstat)
{
  TOSARG(short,dev);
  return internal_Bconstat( dev );
}

BIOSFUNC(Bconin)
{
  TOSARG(short,dev);
  return internal_Bconin( dev );
}

BIOSFUNC(Bconout)
{
  TOSARG(short,dev);
  TOSARG(short,c);
  return internal_Bconout( dev, c );
}

BIOSFUNC(Bcostat)
{
  TOSARG(short,dev);
  return internal_Bcostat( dev );
}

/*
 * These are now the real functions...
 */

long internal_Bconstat( int dev )
{
	int fd;

	/* Cannot read from printer... (and don't open device for this!) */
	if (dev == BIOS_DEV_PRT)
		return 0;
	if ((fd = get_biosdev_fd( dev, 1 )) < 0)
		return 0; /* open error -> no char available */
	return ioready( fd, O_RDONLY );
}

long internal_Bcostat( int dev )
{
	int fd;

	/* MIDI and IKBD are swapped for this function! */
	if (dev == BIOS_DEV_MIDI)
		dev = BIOS_DEV_IKBD;
	else if (dev == BIOS_DEV_IKBD)
		dev = BIOS_DEV_MIDI;

	if ((fd = get_biosdev_fd( dev, 0 )) < 0)
		return 0; /* open error -> cannot send */
	return ioready( fd, O_WRONLY );
}

/* This Bconin() returns errno's, though the original usually does not... */
long internal_Bconin( int dev )
{
	int fd;
	unsigned char c;

	/* Cannot read from printer... (and don't open device for this!) */
	if (dev == BIOS_DEV_PRT)
		return TOS_EBADRQ;
	if ((fd = get_biosdev_fd( dev, 0 )) < 0)
		return TOS_EUNDEV;

	if (read( fd, &c, 1 ) != 1)
		return translate_error( errno );

	if (dev == BIOS_DEV_CON || dev == BIOS_DEV_RAWCON)
		return emulate_scancode( c );
	else
		return c;
}

/* This Bconout() returns errno's, though the original is void (except PRT) */
long internal_Bconout( int dev, int xc )
{
	int fd;
	unsigned char c = xc;

	/* In case of PRT:, protect against races with SIGALRM in here */
	if (dev == BIOS_DEV_PRT)
		sigprocmask( SIG_BLOCK, &ALRM_sigset, NULL );
	
	if ((fd = get_biosdev_fd( dev, 0 )) < 0)
		return TOS_EUNDEV;
	if (dev == BIOS_DEV_CON) {
	  if( Opt_vt52 ) {
		vt52_put_char( c, fd );
	  }
	  else {
		write( fd, &c, 1 );
	  }
	  return 1;
	}
	else if (dev == BIOS_DEV_RAWCON) {
		/*
		 * It'd be hard to correctly print chars < 32, we'd need another
		 * font... And a "\xxx" notation could mess up output due to different
		 * length. A bad solution for all this is to just print a '.' :-( 
		 */
		if (c < ' ') c = '.';
	}
	else if (dev == BIOS_DEV_PRT) {
		/* If writing to the printer, restart the closing timeout */
		start_lpr_timeout();
	}
	return (write( fd, &c, 1 ) == 1);

	if (dev == BIOS_DEV_PRT)
		sigprocmask( SIG_UNBLOCK, &ALRM_sigset, NULL );
}

/*
 * add emulated scancode for ASCII code
 */
static long emulate_scancode( unsigned char c )
{
  return (ascii_to_scancode[ c ] << 8) | c;
}

#if 0
/* dummy... libncurses currently doesn't work for me :-( */
void vt52_put_char( int ch, int fd )
{
	unsigned char c = ch;
	write( fd, &c, 1 );
}
#endif


/*
 * The next two functions solve a problem with the "printer device" under
 * Unix: We cannot use the device directly, but have to go via the spooler
 * (usually 'lpr'). This is ok so far, just we don't know when to close the
 * pipe to the spooler and thus start the job... and doing this only at the
 * end of the program may be too late. The solution is to use a timer
 * (length is an user option), that fires if nothing has been printed for some
 * time. Then the pipe is closed. But printing functions thus have to protect
 * against SIGALRM, otherwise the pipe fd can become invalid.
 */
static void start_lpr_timeout( void )
{
	sigset_t mask;
	struct sigaction sa;

	sigemptyset(&mask);

	sa.sa_mask = mask;
	sa.sa_handler = (__sighandler_t)lpr_times_out;
	sa.sa_flags = SA_RESTART;
	sigaction( SIGALRM, &sa, NULL );
	
	alarm( Opt_lpr_timeout );
}

static void lpr_times_out( void )
{
	/* Turn off further alarms... */
	alarm( 0 );

	/* close lpr pipe */
	pclose( LprPipe );
	BiosDevFd[BIOS_DEV_PRT] = NOT_OPEN;
}


/*
 * This function handles all the details of BIOS device mappings... Inputs are
 * the device number given by the user, and whether it's intended to be used
 * for input (1) or for output (0) (which in most cases makes no
 * difference...). Output is either the appropriate Unix fd, or -1 for error.
 */

static int get_biosdev_fd( int bdev, int i_o )
{
	char *devname;
	int omode, make_pipe, fd;
	
	if (bdev < MIN_BIOS_DEV || bdev > MAX_BIOS_DEV)
		/* non-existant device */
		return -1;

	/* RAWCON is internally used for CON_OUT, while CON is CON_IN; the Unix
	 * fd's for both directions could be different...
	 */
	if (bdev == BIOS_DEV_RAWCON)
		bdev = BIOS_DEV_CON;
	if (bdev == BIOS_DEV_CON)
		bdev = i_o ? BIOS_DEV_CONIN : BIOS_DEV_CONOUT;

	
	if (bdev == BIOS_DEV_AUX)
		bdev = CurrentSerial;

	if (BiosDevFd[bdev] != NOT_OPEN)
		/* device is already open... */
		return( BiosDevFd[bdev] );

	/* must open appropriate device */
	omode = O_RDWR;
	make_pipe = 0;
	switch( bdev ) {

	  case BIOS_DEV_PRT:
		devname = Opt_lpr;
		make_pipe = 1;
		break;

	  case BIOS_DEV_CONIN:
		devname = "/dev/tty";
		omode = O_RDONLY;
		break;

	 case BIOS_DEV_CONOUT:
		devname = "/dev/tty";
		omode = O_WRONLY;
		break;

	  case BIOS_DEV_MIDI:
		devname = Opt_midi_dev;
		break;

	  case BIOS_DEV_IKBD:
		/* device not supported...  */
		return -1;

	  case BIOS_DEV_MOD1:
		devname = Opt_modem1_dev;
		break;

	  case BIOS_DEV_MOD2:
		devname = Opt_modem2_dev;
		break;

	  case BIOS_DEV_SER1:
		devname = Opt_serial1_dev;
		break;

	  case BIOS_DEV_SER2:
		devname = Opt_serial2_dev;
		break;

	  default:
		/* bad device (shouldn't happen)  */
		return -1;
	}

	if (make_pipe) {
		if (!LprPipe && !(LprPipe = popen( devname, "w" ))) {
			fprintf( stderr, "Cannot open pipe to lpr command: %s\n",
					 strerror(errno) );
			return -1;
		}
		fd = fileno(LprPipe);
		/* start the timeout */
		start_lpr_timeout();
	}
	else {
		if ((fd = open( devname, omode )) < 0) {
			fprintf( stderr, "Cannot open %s: %s\n",
					 devname, strerror(errno) );
			return -1;
		}
	}
	
	return BiosDevFd[bdev] = fd;
}

void init_biosdev_fd( void )
{
	int i;

	for( i = MIN_BIOS_DEV; i <= MAX_BIOS_DEV; ++i )
		BiosDevFd[i] = NOT_OPEN;

	/* if stdin/stdout are ttys, use them for CON:; otherwise, open /dev/tty
	 * later */
	if (isatty(0))
		BiosDevFd[BIOS_DEV_CONIN] = 0;
	if (isatty(1))
		BiosDevFd[BIOS_DEV_CONOUT] = 1;

	/* set up ALRM_sigset */
	sigemptyset( &ALRM_sigset );
	sigaddset( &ALRM_sigset, SIGALRM );
}

/* Local Variables:              */
/* tab-width: 4                  */
/* compile-command: "make -C .." */
/* End:                          */
