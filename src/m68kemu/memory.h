#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_DEBUG		1


#define SET_LONG(X,Y)	((*((UInt16 *)(X)) = (((UInt32)(Y)&0xFFFF0000) >> 16)), \
			 (*((UInt16 *)(X) + 1) = (UInt32)(Y)&0x0000FFFF))
#define GET_LONG(X)	(((UInt32)(*(UInt16 *)(X))) << 16 | \
			 ((UInt32)(*((UInt16 *)(X) + 1))))


#define VADDR(X)	((X) >= m68k_memory_base_address && \
			 (X) < m68k_memory_top_address)
#define GADDR(X)	(((UInt8 *)m68k_memory_ptr) + \
			 ((X) - m68k_memory_base_address))


#define xget_byte(X)	(VADDR(X) ? *(UInt8 *)GADDR(X) : reg_get_byte( X ))
#define xget_word(X)	(VADDR(X) ? *(UInt16 *)GADDR(X) : reg_get_word( X ))
#define xget_long(X)	(VADDR(X) ? GET_LONG(GADDR(X)) : reg_get_long( X ))

#define xput_byte(X,Y)	((VADDR(X) ? *(UInt8 *)GADDR(X)=(Y):reg_put_byte(X,Y)))
#define xput_word(X,Y)	((VADDR(X) ? *(UInt16 *)GADDR(X)=(Y):reg_put_word(X,Y)))
#define xput_long(X,Y)	((VADDR(X) ? SET_LONG(GADDR(X),(Y)):reg_put_long(X,Y)))

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
