#ifndef MISC_H
#define MISC_H

#include "memory.h"

typedef char FlagType;


typedef struct {
  int cpu_level;
  int m68k_speed;
  int cpu_compatible;
} CPUPrefs;

typedef struct {
    UInt32 regs[16];
    Ptr32  usp,isp,msp;
    UInt16 sr;
    FlagType t1;
    FlagType t0;
    FlagType s;
    FlagType m;
    FlagType x;
    FlagType stopped;
    int intmask;

    UInt32 pc;
    UInt8 *pc_p;
    UInt8 *pc_oldp;

    UInt32 vbr,sfc,dfc;

    double fp[8];
    UInt32 fpcr,fpsr,fpiar;

    UInt32 spcflags;
    UInt32 kick_mask;

    /* Fellow sources say this is 4 longwords. That's impossible. It needs
     * to be at least a longword. The HRM has some cryptic comment about two
     * instructions being on the same longword boundary.
     * The way this is implemented now seems like a good compromise.
     */
    UInt32 prefetch;
} RegStructType;

#define HAVE_VSPRINTF		1
#undef X86_ASSEMBLY
#undef AMIGA
#undef USE_COMPILER
#undef M68K_FLAG_OPT
#undef ACORN_FLAG_OPT

#define INLINE

#define SPCFLAG_STOP		0x0002
#define SPCFLAG_DISK		0x0004
#define SPCFLAG_INT		0x0008
#define SPCFLAG_BRK		0x0010
#define SPCFLAG_EXTRA_CYCLES	0x0020
#define SPCFLAG_TRACE		0x0040
#define SPCFLAG_DOTRACE		0x0080
#define SPCFLAG_DOINT		0x0100
#define SPCFLAG_BLTNASTY	0x0200
#define SPCFLAG_EXEC		0x0400
#define SPCFLAG_MODE_CHANGE	0x0800






#define MEMORY_DEBUG		1

#define xget_byte(X)	(VADDR(X) ? GET_BYTE(X) : reg_get_byte( X ))
#define xget_word(X)	(VADDR(X) ? GET_WORD(X) : reg_get_word( X ))
#define xget_long(X)	(VADDR(X) ? GET_LONG(X) : reg_get_long( X ))

#define xput_byte(X,Y)	((VADDR(X) ? SET_BYTE((X),(Y)):reg_put_byte(X,Y)))
#define xput_word(X,Y)	((VADDR(X) ? SET_WORD((X),(Y)):reg_put_word(X,Y)))
#define xput_long(X,Y)	((VADDR(X) ? SET_LONG((X),(Y)):reg_put_long(X,Y)))

UInt8 reg_get_byte( Ptr32 );
UInt16 reg_get_word( Ptr32 );
UInt32 reg_get_long( Ptr32 );
int reg_put_byte( Ptr32, UInt8 );
int reg_put_word( Ptr32, UInt16 );
int reg_put_long( Ptr32, UInt32 );

#if MEMORY_DEBUG
static UInt8 get_byte( Ptr32 addr )
{
  extern Ptr32 m68k_memory_base_address;
  extern UInt32 m68k_memory_size;
  extern Ptr32 m68k_memory_top_address;
  extern void *m68k_memory_ptr;
  UInt8 ret = xget_byte( addr );

  printf( "get byte from 0x%08x -> 0x%02x\n", addr, (unsigned int)ret );
  return ret;
}

static UInt16 get_word( Ptr32 addr )
{
  extern Ptr32 m68k_memory_base_address;
  extern UInt32 m68k_memory_size;
  extern Ptr32 m68k_memory_top_address;
  extern void *m68k_memory_ptr;
  UInt16 ret = xget_word( addr );

  printf( "get word from 0x%08x -> 0x%04x\n", addr, (unsigned int)ret );
  return ret;
}

static UInt32 get_long( Ptr32 addr )
{
  extern Ptr32 m68k_memory_base_address;
  extern UInt32 m68k_memory_size;
  extern Ptr32 m68k_memory_top_address;
  extern void *m68k_memory_ptr;
  UInt32 ret = xget_long( addr );

  printf( "get long from 0x%08x -> 0x%08x\n", addr, (unsigned int)ret );
  return ret;
}

static void put_byte( Ptr32 addr, UInt8 value )
{
  extern Ptr32 m68k_memory_base_address;
  extern UInt32 m68k_memory_size;
  extern Ptr32 m68k_memory_top_address;
  extern void *m68k_memory_ptr;

  printf( "set byte 0x%08x <- 0x%02x\n", addr, (unsigned int)value );
  xput_byte( addr, value );
}

static void put_word( Ptr32 addr, UInt16 value )
{
  extern Ptr32 m68k_memory_base_address;
  extern UInt32 m68k_memory_size;
  extern Ptr32 m68k_memory_top_address;
  extern void *m68k_memory_ptr;

  printf( "set word 0x%08x <- 0x%04x\n", addr, (unsigned int)value );
  xput_word( addr, value );
}

static void put_long( Ptr32 addr, UInt32 value )
{
  extern Ptr32 m68k_memory_base_address;
  extern UInt32 m68k_memory_size;
  extern Ptr32 m68k_memory_top_address;
  extern void *m68k_memory_ptr;

  printf( "set long 0x%08x <- 0x%08x\n", addr, (unsigned int)value );
  xput_long( addr, value );
}
#else
#define get_byte(X)		xget_byte(X)
#define get_word(X)		xget_word(X)
#define get_long(X)		xget_long(X)
#define put_byte(X,Y)		xput_byte((X),(Y))
#define put_word(X,Y)		xput_word((X),(Y))
#define put_long(X,Y)		xput_long((X),(Y))
#endif

#define get_real_address(X)		((void *)(X))

void init_memory( void );


#endif
