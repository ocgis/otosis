#ifndef MEMORY_H
#define MEMORY_H

#define VADDR(X)	((X) >= m68k_memory_base_address && \
			 (X) < m68k_memory_top_address)
#define GADDR(X)	(((UInt8 *)m68k_memory_ptr) + \
			 ((X) - m68k_memory_base_address))

#define D(X)		(printf("getting from 0x%08xn", (X)))

#define get_byte(X)	(D(X),(VADDR(X) ? *(UInt8 *)GADDR(X) : reg_get_byte( X )))
#define get_word(X)	(D(X),(VADDR(X) ? *(UInt16 *)GADDR(X) : reg_get_word( X )))
#define get_long(X)	(D(X),(VADDR(X) ? *(UInt32 *)GADDR(X) : reg_get_long( X )))

#define D2(X,Y)		(printf("setting 0x%08xto 0x%08x\n", (X), (unsigned int)(Y)))
#define put_byte(X,Y)	(D2(X,(UInt8)Y),(VADDR(X) ? *(UInt8 *)GADDR(X)=(Y):reg_put_byte(X,Y)))
#define put_word(X,Y)	(D2(X,(UInt16)Y),(VADDR(X) ? *(UInt16 *)GADDR(X)=(Y):reg_put_word(X,Y)))
#define put_long(X,Y)	(D2(X,(UInt32)Y),(VADDR(X) ? *(UInt32 *)GADDR(X)=(Y):reg_put_long(X,Y)))

#define get_real_address(X)		((void *)(X))

void init_memory( void );
UInt8 reg_get_byte( Ptr32 );
UInt16 reg_get_word( Ptr32 );
UInt32 reg_get_long( Ptr32 );
int reg_put_byte( Ptr32, UInt8 );
int reg_put_word( Ptr32, UInt16 );
int reg_put_long( Ptr32, UInt32 );

#endif
