#ifndef MEMORY_H
#define MEMORY_H



#define HAVE_INT64		1

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;

typedef signed char SInt8;
typedef signed short SInt16;
typedef signed int SInt32;

#if HAVE_INT64
typedef unsigned long long UInt64;
typedef signed long long SInt64;
#define VAL64(X)		(X ## LL)
#define UVAL64(X)		(X ## ULL)
#endif

#ifdef NATIVE_M68K
typedef void *Ptr32;
#else
typedef UInt32 Ptr32;
#endif





#ifdef NATIVE_M68K

#define GET_LONG(X)	(X)
#define GET_WORD(X)	(X)
#define GET_BYTE(X)	(X)
#define SET_LONG(X,Y)	((X) = (Y))
#define SET_WORD(X,Y)	((X) = (Y))
#define SET_BYTE(X,Y)	((X) = (Y))

#else

/*
 *  The following code only works on big endian machines
 */
#define VADDR(X)	((X) >= m68k_memory_base_address && \
			 (X) < m68k_memory_top_address)
#define GADDR(X)	(((UInt8 *)m68k_memory_ptr) + \
			 ((X) - m68k_memory_base_address))

#define SET_LONG(X,Y)	((*((UInt16 *)(GADDR(X))) = (((UInt32)(Y)&0xFFFF0000) >> 16)), \
			 (*((UInt16 *)(GADDR(X)) + 1) = (UInt32)(Y)&0x0000FFFF))
#define SET_WORD(X,Y)	(*(UInt16 *)(GADDR(X)) = (Y))
#define SET_BYTE(X,Y)	(*(UInt8 *)(GADDR(X)) = (Y))

#define GET_LONG(X)	(((UInt32)(*(UInt16 *)(GADDR(X)))) << 16 | \
			 ((UInt32)(*((UInt16 *)(GADDR(X)) + 1))))
#define GET_WORD(X)	(*(UInt16 *)(GADDR(X)))
#define GET_BYTE(X)	(*(UInt8 *)(GADDR(X)))

#endif




#endif
