#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

CPUPrefs cpu_prefs;
int debugging = 0;

int main( void )
{
  cpu_prefs.m68k_speed = 0;
  cpu_prefs.cpu_level = 3;
  cpu_prefs.cpu_compatible = 0;

  puts( "not yet implemented" );
  return 0;
}

void debug( void )
{
  printf( "debug()\n" );
}

void compiler_flush_jsr_stack( void )
{
}

void quit_program( void )
{
  exit( 0 );
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
