 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68000 emulation
  *
  * Copyright 1995 Bernd Schmidt
  */

#include "misc.h"
#include "machdep/maccess.h"

#define ASM_SYM_FOR_FUNC(X)

extern int areg_byteinc[];
extern int imm8_table[];

extern int movem_index1[256];
extern int movem_index2[256];
extern int movem_next[256];

extern int fpp_movem_index1[256];
extern int fpp_movem_index2[256];
extern int fpp_movem_next[256];

extern int broken_in;

typedef unsigned long cpuop_func (UInt32);

struct cputbl {
    cpuop_func *handler;
    int specific;
    UInt16 opcode;
};

extern unsigned long op_illg (UInt32);

typedef char flagtype;

extern struct regstruct
{
    UInt32 regs[16];
    Ptr32  usp,isp,msp;
    UInt16 sr;
    flagtype t1;
    flagtype t0;
    flagtype s;
    flagtype m;
    flagtype x;
    flagtype stopped;
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
} regs, lastint_regs;

#define m68k_dreg(r,num) ((r).regs[(num)])
#define m68k_areg(r,num) (((r).regs + 8)[(num)])

#define get_ibyte(o) do_get_mem_byte((UInt8 *)(regs.pc_p + (o) + 1))
#define get_iword(o) do_get_mem_word((UInt16 *)(regs.pc_p + (o)))
#define get_ilong(o) do_get_mem_long((UInt32 *)(regs.pc_p + (o)))

#ifdef HAVE_GET_WORD_UNSWAPPED
#define GET_OPCODE (do_get_mem_word_unswapped (regs.pc_p))
#else
#define GET_OPCODE (get_iword (0))
#endif

static INLINE UInt32 get_ibyte_prefetch (SInt32 o)
{
    if (o > 3 || o < 0)
	return do_get_mem_byte((UInt8 *)(regs.pc_p + o + 1));

    return do_get_mem_byte((UInt8 *)(((UInt8 *)&regs.prefetch) + o + 1));
}
static INLINE UInt32 get_iword_prefetch (SInt32 o)
{
    if (o > 3 || o < 0)
	return do_get_mem_word((UInt16 *)(regs.pc_p + o));

    return do_get_mem_word((UInt16 *)(((UInt8 *)&regs.prefetch) + o));
}
static INLINE UInt32 get_ilong_prefetch (SInt32 o)
{
    if (o > 3 || o < 0)
	return do_get_mem_long((UInt32 *)(regs.pc_p + o));
    if (o == 0)
	return do_get_mem_long(&regs.prefetch);
    return (do_get_mem_word (((UInt16 *)&regs.prefetch) + 1) << 16) | do_get_mem_word ((UInt16 *)(regs.pc_p + 4));
}

#define m68k_incpc(o) (regs.pc_p += (o))

static INLINE void fill_prefetch_0 (void)
{
    UInt32 r;
#ifdef UNALIGNED_PROFITABLE
    r = *(UInt32 *)regs.pc_p;
    regs.prefetch = r;
#else
    r = do_get_mem_long ((UInt32 *)regs.pc_p);
    do_put_mem_long (&regs.prefetch, r);
#endif
}

#if 0
static INLINE void fill_prefetch_2 (void)
{
    UInt32 r = do_get_mem_long (&regs.prefetch) << 16;
    UInt32 r2 = do_get_mem_word (((UInt16 *)regs.pc_p) + 1);
    r |= r2;
    do_put_mem_long (&regs.prefetch, r);
}
#else
#define fill_prefetch_2 fill_prefetch_0
#endif

/* These are only used by the 68020/68881 code, and therefore don't
 * need to handle prefetch.  */
static INLINE UInt32 next_ibyte (void)
{
    UInt32 r = get_ibyte (0);
    m68k_incpc (2);
    return r;
}

static INLINE UInt32 next_iword (void)
{
    UInt32 r = get_iword (0);
    m68k_incpc (2);
    return r;
}

static INLINE UInt32 next_ilong (void)
{
    UInt32 r = get_ilong (0);
    m68k_incpc (4);
    return r;
}

#if !defined USE_COMPILER
static INLINE void m68k_setpc (Ptr32 newpc)
{
    regs.pc_p = regs.pc_oldp = get_real_address(newpc);
    regs.pc = newpc;
}
#else
extern void m68k_setpc (Ptr32 newpc);
#endif

static INLINE Ptr32 m68k_getpc (void)
{
    return regs.pc + ((char *)regs.pc_p - (char *)regs.pc_oldp);
}

static INLINE Ptr32 m68k_getpc_p (UInt8 *p)
{
    return regs.pc + ((char *)p - (char *)regs.pc_oldp);
}

#ifdef USE_COMPILER
extern void m68k_setpc_fast (Ptr32 newpc);
extern void m68k_setpc_bcc (Ptr32 newpc);
extern void m68k_setpc_rte (Ptr32 newpc);
#else
#define m68k_setpc_fast m68k_setpc
#define m68k_setpc_bcc  m68k_setpc
#define m68k_setpc_rte  m68k_setpc
#endif

static INLINE void m68k_setstopped (int stop)
{
    regs.stopped = stop;
    if (stop)
	regs.spcflags |= SPCFLAG_STOP;
}

static INLINE UInt32 get_disp_ea_020 (UInt32 base, UInt32 dp)
{
    int reg = (dp >> 12) & 15;
    SInt32 regd = regs.regs[reg];
    if ((dp & 0x800) == 0)
	regd = (SInt32)(SInt16)regd;
    regd <<= (dp >> 9) & 3;
    if (dp & 0x100) {
	SInt32 outer = 0;
	if (dp & 0x80) base = 0;
	if (dp & 0x40) regd = 0;

	if ((dp & 0x30) == 0x20) base += (SInt32)(SInt16)next_iword();
	if ((dp & 0x30) == 0x30) base += next_ilong();

	if ((dp & 0x3) == 0x2) outer = (SInt32)(SInt16)next_iword();
	if ((dp & 0x3) == 0x3) outer = next_ilong();

	if ((dp & 0x4) == 0) base += regd;
	if (dp & 0x3) base = get_long (base);
	if (dp & 0x4) base += regd;

	return base + outer;
    } else {
	return base + (SInt32)((SInt8)dp) + regd;
    }
}
static INLINE UInt32 get_disp_ea_000 (UInt32 base, UInt32 dp)
{
    int reg = (dp >> 12) & 15;
    SInt32 regd = regs.regs[reg];
#if 1
    if ((dp & 0x800) == 0)
	regd = (SInt32)(SInt16)regd;
    return base + (SInt8)dp + regd;
#else
    SInt32 regd16;
    UInt32 mask;
    mask = ((dp & 0x800) >> 11) - 1;
    regd16 = (SInt32)(SInt16)regd;
    regd16 &= mask;
    mask = ~mask;
    base += (SInt8)dp;
    regd &= mask;
    regd |= regd16;
    return base + regd;
#endif
}



static INLINE void m68k_do_rts(void)
{
    m68k_setpc(get_long(m68k_areg(regs, 7)));
    m68k_areg(regs, 7) += 4;
}

static INLINE void m68k_do_bsr(Ptr32 oldpc, SInt32 offset)
{
    m68k_areg(regs, 7) -= 4;
    put_long(m68k_areg(regs, 7), oldpc);
    m68k_incpc(offset);
}

static INLINE void m68k_do_jsr(Ptr32 oldpc, Ptr32 dest)
{
    m68k_areg(regs, 7) -= 4;
    put_long(m68k_areg(regs, 7), oldpc);
    m68k_setpc(dest);
}






extern SInt32 ShowEA (int reg, amodes mode, wordsizes size, char *buf);

extern void MakeSR (void);
extern void MakeFromSR (void);
extern void Exception (int, Ptr32);
extern void dump_counts (void);
extern void m68k_move2c (int, UInt32 *);
extern void m68k_movec2 (int, UInt32 *);
extern void m68k_divl (UInt32, UInt32, UInt16, Ptr32);
extern void m68k_mull (UInt32, UInt32, UInt16);
extern void init_m68k (void);
extern void m68k_go (int);
extern void m68k_dumpstate (Ptr32 *);
extern void m68k_disasm (Ptr32, Ptr32 *, int);
extern void m68k_reset (void);

extern void mmu_op (UInt32, UInt16);

extern void fpp_opp (UInt32, UInt16);
extern void fdbcc_opp (UInt32, UInt16);
extern void fscc_opp (UInt32, UInt16);
extern void ftrapcc_opp (UInt32,Ptr32);
extern void fbcc_opp (UInt32, Ptr32, UInt32);
extern void fsave_opp (UInt32);
extern void frestore_opp (UInt32);

/* Opcode of faulting instruction */
extern UInt16 last_op_for_exception_3;
/* PC at fault time */
extern Ptr32 last_addr_for_exception_3;
/* Address that generated the exception */
extern Ptr32 last_fault_for_exception_3;

#define CPU_OP_NAME(a) op ## a

/* 68020 + 68881 */
extern struct cputbl op_smalltbl_0[];
/* 68020 */
extern struct cputbl op_smalltbl_1[];
/* 68010 */
extern struct cputbl op_smalltbl_2[];
/* 68000 */
extern struct cputbl op_smalltbl_3[];
/* 68000 slow but compatible.  */
extern struct cputbl op_smalltbl_4[];

extern cpuop_func *cpufunctbl[65536] ASM_SYM_FOR_FUNC ("cpufunctbl");

