/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
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

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <libotosis.h>
#include <libotosis_option.h>

#include "debug.h"

#ifdef DEBUG_STACK
unsigned long start_stack;		/* for catching stack overruns */
#endif


static int relocate_program( TosBasepage *bp, TosExecHeader *hdr, FILE *fp );

TosProgram *load_tos_program( char *filename )
{
  TosProgram * prog;
  TosExecHeader *hdr;
  TosBasepage *bp;
  void *prg, *buf;
  ulong size;
  FILE *fp;
  char *env, *src, *dest;
  char ascii_num[ 4 ];

  /* Allocate a new program structure */
  prog = new_program ();

  /* Set up the MiNT emulation flag */
  prog->emulate_mint = Opt_MiNT;

  /* Set up GEM flag, if program name ends on .app or .prg
	 and Opt_GEM is not set to "no" */
  /* Use case insensitive compare */
  if(Opt_GEM && (!strncasecmp(filename+strlen(filename)-4, ".app", 4) ||
				 !strncasecmp(filename+strlen(filename)-4, ".prg", 4)))
	prog->gem = 1;
  else
	prog->gem = 0;
  
  /* Set up the environment */
  DDEBUG( "Environment: %s\n", Opt_environment );
  if( strcmp( Opt_environment, "-" ) == 0 ) {
	env = NULL;
  }
  else {
	env = mymalloc( strlen( Opt_environment ) + 2 );
	for( src = Opt_environment, dest = env ; *src ; src++ ) {
	  if( *src == '\\' ) {
		src++;
		if( *src >= '0' && *src <= '9' ) {
		  ascii_num[ 0 ] = *src++;
		  ascii_num[ 1 ] = *src++;
		  ascii_num[ 2 ] = *src;
		  ascii_num[ 3 ] = 0;
		  *dest++ = strtol( ascii_num, NULL, 8 );
		}
		else {
		  switch( *src ) {
		  case 'n':
			*dest++ = '\n';
			break;
		  case 'r':
			*dest++ = '\r';
			break;
		  default:
			*dest++ = *src;
			break;
		  }
		}
	  }
	  else if( *src == ',' ) {
		*dest++ = '\0';
	  }
	  else {
		*dest++ = *src;
	  }
	}
	*dest++ = 0;
	*dest++ = 0;
  }  

  /* Set up the trace flag */
  prog->trace = Opt_trace;

  if( (fp = fopen( filename, "r" )) == NULL ){
    fprintf( stderr, "Can't open %s: %s\n", filename, strerror(errno) );
    return NULL;
  }

  /* read the header */
  hdr = mymalloc( sizeof(TosExecHeader) );
  if (fread( hdr, sizeof(TosExecHeader), 1, fp ) != 1 ||
	  ntohs(hdr->magic) != TOS_PROGRAM_MAGIC) {
	fprintf( stderr, "%s: no GEMDOS executable\n", filename );
	free( hdr );
	fclose( fp );
	return NULL;
  }

  /* calculate size of TPA */
  size = sizeof(TosBasepage) + ntohl(hdr->tsize) + ntohl(hdr->dsize) +
    ntohl(hdr->bsize) + Opt_extra_mem*1024;
  buf = mymalloc( size );
  bp = (TosBasepage *)buf;
  prg = (void *)(bp + 1);

  /* read program text + data */
  if(fread(prg, sizeof(char), ntohl(hdr->tsize) + ntohl(hdr->dsize), fp ) !=
     ntohl(hdr->tsize) + ntohl(hdr->dsize))
  {
	  fprintf( stderr, "%s: short executable!\n", filename );
	err_ret:
	  free( buf );
	  free( hdr );
	  fclose( fp );
	  return NULL;
  }

  /* initialize basepage */
  bp->lowtpa = htonl((UInt32)bp);
  bp->hitpa = htonl((UInt32)buf + size);
  bp->tbase = htonl(TEXT_SEGMENT(hdr,prg));
  bp->tlen = hdr->tsize;
  bp->dbase = htonl(DATA_SEGMENT(hdr,prg));
  bp->dlen = hdr->dsize;
  bp->bbase = htonl(BSS_SEGMENT(hdr,prg));
  bp->blen = hdr->bsize;
  bp->parent = (Ptr32)NULL;
  bp->env = htonl(env);
  bp->cmdlin[ 0 ] = 0;

  /*  patch_program( hdr ); */
  if (relocate_program( bp, hdr, fp ) < 0) {
	  fprintf( stderr, "%s: relocation failed\n", filename );
	  goto err_ret;
  }
  fclose( fp );
  
  /* clear BSS and maybe rest of TPA */
  memset((void *)BSS_SEGMENT(hdr,prg), 0,
         ntohl(hdr->bsize) +
         ((ntohl(hdr->prgflags) & TOS_PRGFLAG_FASTLOAD) ? 0 : Opt_extra_mem*1024) );

  prog->text = TEXT_SEGMENT(hdr,prg);
  prog->data = DATA_SEGMENT(hdr,prg);
  prog->bss = BSS_SEGMENT(hdr,prg);
  prog->size = size;

  prog->basepage = bp;
  prog->basepage->dta = (Dta *)prog->basepage->cmdlin;
  prog->dta = ntohl(prog->basepage->dta);

  /* The program starts in user mode */
  prog->super = 0;
  
  /* The program starts in the TOS domain */
  prog->domain = 0;

#ifdef DEBUG_STACK
  start_stack = BSS_SEGMENT(hdr,prg) + ntohl(hdr->bsize);
#endif

  free(hdr);
  
  return prog;
}

static int relocate_program( TosBasepage *bp, TosExecHeader *hdr, FILE *fp )
{
  unsigned char *fix, *fixaddr;
  unsigned long offset, reloc, space, n;

  if(hdr->absflag)  return 0;
  reloc = ntohl((ulong)bp->tbase);
  /* seek to reloc table and read first offset */
  fseek( fp, (ulong)FIXUP_OFFSET(hdr), SEEK_SET );
  if (fread( &offset, sizeof(ulong), 1, fp ) != 1)
	  return -1;
  if (!offset) return 0;

  /* fix first offset */
  fixaddr = (unsigned char *)(reloc + ntohl(offset));
  *(ulong *)fixaddr = htonl(ntohl(*(ulong *)fixaddr) + reloc);

  /* available space for reloc buffer */
  space = ntohl(bp->hitpa) - BSS_SEGMENT(hdr,reloc);
  
  for(;;) {
    /* read as much of reloc table as possible into BSS */
    fix = (unsigned char *)BSS_SEGMENT(hdr,reloc);
	if (!(n = fread( fix, sizeof(char), space, fp )))
	  break;
	else if (n < 0) {
	  perror( "fread" );
	  return -1;
    }

	for( ; *fix && n > 0; ++fix, --n ) {
	  if (*fix == 1)
      {
        fixaddr += 254;
      }
	  else
      {
        fixaddr += *fix;
        *(ulong *)fixaddr = htonl(ntohl(*(ulong *)fixaddr) + reloc);
      }
    }
  }
  return 0;
}

/* Local Variables:              */
/* tab-width: 4                  */
/* End:                          */
