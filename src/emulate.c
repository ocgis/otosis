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

#include <netinet/in.h>
#include <oaesis.h>
#include <ocpuis.h>
#include <stdio.h>

#include "emulate.h"


static TosProgram * my_prog = NULL;

static
CPUbyte
my_get_byte(CPUaddr addr)
{
  return *((CPUbyte *)addr);
}


static
CPUword
my_get_word(CPUaddr addr)
{
  return ntohs(*((CPUword *)addr));
}


static
CPUlong
my_get_long(CPUaddr addr)
{
  return ntohl(*((CPUlong *)addr));
}


static
void
my_put_byte(CPUaddr addr,
            CPUbyte value)
{
  *((CPUbyte *)addr) = value;
}


static
void
my_put_word(CPUaddr addr,
            CPUword value)
{
  *((CPUword *)addr) = htons(value);
}


static
void
my_put_long(CPUaddr addr,
            CPUlong value)
{
  *((CPUlong *)addr) = htonl(value);
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
  oldpc = CPUget_pc();

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

      d0 = dispatch_gemdos(usp);
      
      CPUset_dreg(0, d0);
      break;

    case 2:
      switch(CPUget_dreg(0) & 0xffff)
      {
      case 115: /* VDI */
        vdi_call(CPUget_dreg(1));
        break;

      case 200: /* AES */
        aes_call(CPUget_dreg(1));
        break;

      default:
        fprintf(stderr, "Illegal Xgemdos call: 0x%x\n", CPUget_dreg(0));
      }
      break;

    case 13:     /* Bios, trap #13 */
      usp = CPUget_usp();

      d0 = dispatch_bios(usp);
      
      CPUset_dreg(0, d0);
      break;

    case 14:     /* Xbios, trap #14 */
      usp = CPUget_usp();

      d0 = dispatch_xbios(usp);
      
      CPUset_dreg(0, d0);
      break;

    default:
      printf( "cannot handle TRAP #%d\n", trap_num);
      bombs(32 + trap_num);
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

  /* Setup a callback handler for oAESis */
  Oaesis_callback_handler(handle_callback);

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
