#include <stdio.h>
#include <stdlib.h>

#include "misc.h"
#include "memory.h"

void dump_registers( void );

extern RegStructType regs;
extern Ptr32 m68k_memory_base_address;
extern UInt32 m68k_memory_size;
extern Ptr32 m68k_memory_top_address;
extern void *m68k_memory_ptr;

CPUPrefs cpu_prefs;
int debugging = 0;
int quit_program = 0;

int main( void )
{
  cpu_prefs.m68k_speed = 0;
  cpu_prefs.cpu_level = 3;
  cpu_prefs.cpu_compatible = 0;

  init_memory();
  init_m68k();

  regs.pc = m68k_memory_base_address;
  regs.pc_p = m68k_memory_ptr;
  dump_registers();

  m68k_go( 1 );

  return 0;
}

void debug( void )
{
  printf( "debug()\n" );
}

void compiler_flush_jsr_stack( void )
{
}

void reset_all_systems( void )
{
  printf( "reset_all_systems()\n" );
}

void customreset( void )
{
  printf( "customreset()\n" );
}

void reset_frame_rate_hack( void )
{
  printf( "reset_frame_rate_hack()\n" );
}

void do_cycles( int n )
{
  printf( "do_cycles( %d )\n", n );
}

void call_calltrap( UInt16 n )
{
  printf( "call_calltrap( 0x%04x )\n", (int)n );
}

void dump_registers( void )
{
  printf( "d0=0x%08x d1=0x%08x d2=0x%08x d3=0x%08x\n",
	  regs.regs[0], regs.regs[1], regs.regs[2], regs.regs[3] );
  printf( "d4=0x%08x d5=0x%08x d6=0x%08x d7=0x%08x\n",
	  regs.regs[4], regs.regs[5], regs.regs[6], regs.regs[7] );
  printf( "a0=0x%08x a1=0x%08x a2=0x%08x a3=0x%08x\n",
	  regs.regs[8], regs.regs[9], regs.regs[10], regs.regs[11] );
  printf( "a4=0x%08x a5=0x%08x a6=0x%08x a7=0x%08x\n",
	  regs.regs[12], regs.regs[13], regs.regs[14], regs.regs[15] );
  printf( "pc=0x%08x usp=0x%08x isp=0x%08x msp=0%08x\n",
	  regs.pc, regs.usp, regs.isp, regs.msp );
}
