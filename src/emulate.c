/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <linea.h>
#include <netinet/in.h>
#ifdef USE_GEM
#include <oaesis.h>
#endif /* USE_GEM */
#include <ocpuis.h>
#include <stdio.h>

#include "emulate.h"
#include "traps.h"

/* FIXME: Maybe it's possible to use CPUexception(2) to signal bus error */

#define TOP_OF_SYSVAR 0x5B3

static
char
sysvar[TOP_OF_SYSVAR + 1];

static
inline
CPUaddr
convert_addr(CPUaddr addr)
{
  if(addr <= TOP_OF_SYSVAR)
  {
    return (CPUaddr)sysvar + addr;
  }
  else
  {
    return addr;
  }
}


static TosProgram * my_prog = NULL;

static
CPUbyte
my_get_byte(CPUaddr addr)
{
  return *((CPUbyte *)convert_addr(addr));
}


static
CPUword
my_get_word(CPUaddr addr)
{
  return ntohs(*((CPUword *)convert_addr(addr)));
}


static
CPUlong
my_get_long(CPUaddr addr)
{
  return ntohl(*((CPUlong *)convert_addr(addr)));
}


static
void
my_put_byte(CPUaddr addr,
            CPUbyte value)
{
  *((CPUbyte *)convert_addr(addr)) = value;
}


static
void
my_put_word(CPUaddr addr,
            CPUword value)
{
  *((CPUword *)convert_addr(addr)) = htons(value);
}


static
void
my_put_long(CPUaddr addr,
            CPUlong value)
{
  *((CPUlong *)convert_addr(addr)) = htonl(value);
}


static
void *
my_real_addr(CPUaddr addr)
{
  return (void *)addr;
}


static
int
my_valid_addr(CPUaddr addr,
              CPUlong size)
{
  if((addr < ntohl(my_prog->basepage->lowtpa)) ||
     (addr + size >= ntohl(my_prog->basepage->hitpa)))
  {
    return 0;
  }
  else
  {
    return 1;
  }
}


static
void
my_handle_exception(int     nr,
                    CPUaddr oldpc)
{
  if(nr > 32)
  {
    CPUaddr usp;
    CPUlong d0;
    int     trap_num;
    
    trap_num = nr - 32;
    switch(trap_num)
    {
    case 1:      /* Gemdos, trap #1 */
      usp = CPUget_usp();

      d0 = dispatch_gemdos((char *)usp);
      
      CPUset_dreg(0, d0);
      break;

    case 2:
#ifdef USE_GEM
      switch(CPUget_dreg(0) & 0xffff)
      {
      case 115: /* VDI */
        vdi_call_be32(CPUget_dreg(1));
        break;

      case 200: /* AES */
        aes_call_be32(CPUget_dreg(1));
        break;

      default:
	fprintf(stderr, "Illegal Xgemdos call: 0x%lx\n", CPUget_dreg(0));
      }
#else /* USE_GEM */
      fprintf(stderr, "Xgemdos not supported in this build.\n");
#endif /* USE_GEM */
      break;

    case 13:     /* Bios, trap #13 */
      usp = CPUget_usp();

      d0 = dispatch_bios((char *)usp);
      
      CPUset_dreg(0, d0);
      break;

    case 14:     /* Xbios, trap #14 */
      usp = CPUget_usp();

      d0 = dispatch_xbios((char *)usp);
      
      CPUset_dreg(0, d0);
      break;

    default:
      printf( "cannot handle TRAP #%d\n", trap_num);
      bombs(32 + trap_num);
    }
  }
  else if(nr == 10) /* LineA */
  {
    int instruction;

    instruction = my_get_word(oldpc);

    switch(instruction)
    {
    case 0xa000:
      linea0();
      CPUset_dreg(0, (CPUlong)__aline);  /* Line-A variable table */
      CPUset_areg(0, (CPUlong)__aline);  /* Line-A variable table */
      CPUset_areg(1, 0);                 /* FIXME: System font headers */
      CPUset_areg(2, 0);                 /* FIXME: Line-A functions table */
      break;

    default:
      fprintf(stderr, "Fuck LineA from behind (0x%lx: 0x%04x)\n",
              oldpc,
              instruction);
    }
  }
  else
  {
    fprintf(stderr, "An exception %d occured\n", nr);
    exit(-1);
  }
}


static
void
my_handle_event(unsigned long count)
{
}


static
void
fill_template(CPU * cpu)
{
  cpu->get_byte = my_get_byte;
  cpu->get_word = my_get_word;
  cpu->get_long = my_get_long;
  cpu->put_byte = my_put_byte;
  cpu->put_word = my_put_word;
  cpu->put_long = my_put_long;
  cpu->real_addr = my_real_addr;
  cpu->valid_addr = my_valid_addr;
  cpu->handle_exception = my_handle_exception;
  cpu->handle_event = my_handle_event;
}


/*
** Description
** Handle callback from oAESis resources
*/
static
int
handle_callback(void * subroutine,
                long   argument)
{
  CPUlong sp;

  sp = CPUget_areg(7);
  sp -= 4;
  my_put_long((CPUaddr)sp, argument);
  CPUset_areg(7, sp);

  CPUsubroutine((CPUaddr)subroutine);

  sp += 4;
  CPUset_areg(7, sp);

  return CPUget_dreg(0);
}


void
emulate(TosProgram * prog)
{
  CPU *   cpu;
  CPUaddr sp;
  my_prog = prog;

#ifdef USE_GEM
  /* Setup path mode for oAESis to MiNT paths */
  Oaesis_set_path_mode(OAESIS_PATH_MODE_MINT);

  /* Emulated clients always use big endian */
  Oaesis_set_client_endian(OAESIS_ENDIAN_BIG);

  /* Setup a callback handler for oAESis */
  Oaesis_callback_handler(handle_callback);
#endif /* USE_GEM */

  /* Setup a pointer to basepage and clear one long */
  sp = ntohl((CPUaddr)prog->basepage->hitpa);
  (CPUbyte *)sp -= 4;
  my_put_long(sp, (CPUlong)prog->basepage);
  (CPUbyte *)sp -= 4;
  my_put_long(sp, (CPUlong)0);

  cpu = CPUsimple_template((CPUaddr)(prog->text),
			   sp,
			   0);

  fill_template(cpu);

  CPUinit(cpu);
  CPUrun();
}
