 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * Memory access functions
  *
  * Copyright 1996 Bernd Schmidt
  */

#ifndef MACCESS_H
#define MACCESS_H

static INLINE UInt32 do_get_mem_long(UInt32 *a)
{
    UInt8 *b = (UInt8 *)a;
    
    return (*b << 24) | (*(b+1) << 16) | (*(b+2) << 8) | (*(b+3));
}

static INLINE UInt16 do_get_mem_word(UInt16 *a)
{
    UInt8 *b = (UInt8 *)a;
    
    return (*b << 8) | (*(b+1));
}

static INLINE UInt8 do_get_mem_byte(UInt8 *a)
{
    return *a;
}

static INLINE void do_put_mem_long(UInt32 *a, UInt32 v)
{
    UInt8 *b = (UInt8 *)a;
    
    *b = v >> 24;
    *(b+1) = v >> 16;
    *(b+2) = v >> 8;
    *(b+3) = v;
}

static INLINE void do_put_mem_word(UInt16 *a, UInt16 v)
{
    UInt8 *b = (UInt8 *)a;
    
    *b = v >> 8;
    *(b+1) = v;
}

static INLINE void do_put_mem_byte(UInt8 *a, UInt8 v)
{
    *a = v;
}

#endif
