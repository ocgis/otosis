#include <stdio.h>

#include "misc.h"
#include "memory.h"
#include "malloc.h"


Ptr32 m68k_memory_base_address;
UInt32 m68k_memory_size;
Ptr32 m68k_memory_top_address;
void *m68k_memory_ptr;


void init_memory( void )
{
  m68k_memory_base_address = 0x100000;
  m68k_memory_size = 0x1000000;
  m68k_memory_top_address = m68k_memory_base_address + m68k_memory_size;
  m68k_memory_ptr = xmalloc( m68k_memory_size );

  {
    UInt16 *p = m68k_memory_ptr;

    *p++ = 0x303c; /* move.w #$abcd,d0 */
    *p++ = 0xabcd;
    *p++ = 0x0640; /* add.w #$10,d0 */
    *p++ = 0x0010;
    *p++ = 0x33c0; /* move.w d0,$101000.l */
    *p++ = 0x0010;
    *p++ = 0x1000;
    *p++ = 0x2c79; /* move.l $100FFE,a6 */
    *p++ = 0x0ffe;
    *p++ = 0x1000;
    *p++ = 0x4e41; /* trap #1 */
  }
}


UInt8 reg_get_byte( Ptr32 addr )
{
  printf( "register read byte from 0x%08x\n", addr );
  return 0;
}

UInt16 reg_get_word( Ptr32 addr )
{
  printf( "register read word from 0x%08x\n", addr );
  return 0;
}

UInt32 reg_get_long( Ptr32 addr )
{
  printf( "register read long from 0x%08x\n", addr ); 
  return 0;
}

int reg_put_byte( Ptr32 addr, UInt8 value )
{
  printf( "register write byte 0x%02x to 0x%08x\n", (int)value, addr );
  return 0;
}

int reg_put_word( Ptr32 addr, UInt16 value )
{
  printf( "register write word 0x%04x to 0x%08x\n", (int)value, addr );
  return 0;
}

int reg_put_long( Ptr32 addr, UInt32 value )
{
  printf( "register write long 0x%08x to 0x%08x\n", (int)value, addr );
  return 0;
}
