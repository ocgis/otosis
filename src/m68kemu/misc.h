#ifndef MISC_H
#define MISC_H

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

typedef UInt32 Ptr32;

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

#endif
