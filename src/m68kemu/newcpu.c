 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68000 emulation
  *
  * (c) 1995 Bernd Schmidt
  */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "machdep/m68k.h"
#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"

extern CPUPrefs cpu_prefs;
extern int quit_program;
extern int debugging;

/* Opcode of faulting instruction */
UInt16 last_op_for_exception_3;
/* PC at fault time */
Ptr32 last_addr_for_exception_3;
/* Address that generated the exception */
Ptr32 last_fault_for_exception_3;

int areg_byteinc[] = { 1,1,1,1,1,1,1,2 };
int imm8_table[] = { 8,1,2,3,4,5,6,7 };

int movem_index1[256];
int movem_index2[256];
int movem_next[256];

int fpp_movem_index1[256];
int fpp_movem_index2[256];
int fpp_movem_next[256];

cpuop_func *cpufunctbl[65536];

#define COUNT_INSTRS 0

#if COUNT_INSTRS
static unsigned long int instrcount[65536];
static UInt16 opcodenums[65536];

static int compfn (const void *el1, const void *el2)
{
    return instrcount[*(const UInt16 *)el1] < instrcount[*(const UInt16 *)el2];
}

static char *icountfilename (void)
{
    char *name = getenv ("INSNCOUNT");
    if (name)
	return name;
    return COUNT_INSTRS == 2 ? "frequent.68k" : "insncount";
}

void dump_counts (void)
{
    FILE *f = fopen (icountfilename (), "w");
    unsigned long int total;
    int i;

    printf ("Writing instruction count file...\n");
    for (i = 0; i < 65536; i++) {
	opcodenums[i] = i;
	total += instrcount[i];
    }
    qsort (opcodenums, 65536, sizeof(UInt16), compfn);

    fprintf (f, "Total: %lu\n", total);
    for (i=0; i < 65536; i++) {
	unsigned long int cnt = instrcount[opcodenums[i]];
	struct instr *dp;
	struct mnemolookup *lookup;
	if (!cnt)
	    break;
	dp = table68k + opcodenums[i];
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
	    ;
	fprintf (f, "%04x: %lu %s\n", opcodenums[i], cnt, lookup->name);
    }
    fclose (f);
}
#else
void dump_counts (void)
{
}
#endif

int broken_in;

static INLINE unsigned int cft_map (unsigned int f)
{
#ifndef HAVE_GET_WORD_UNSWAPPED
    return f;
#else
    return ((f >> 8) & 255) | ((f & 255) << 8);
#endif
}

static unsigned long op_illg_1 (UInt32 opcode);

static unsigned long op_illg_1 (UInt32 opcode)
{
    op_illg (cft_map (opcode));
    return 4;
}

static void build_cpufunctbl (void)
{
    int i;
    unsigned long opcode;
    struct cputbl *tbl = (cpu_prefs.cpu_level == 3 ? op_smalltbl_0
			  : cpu_prefs.cpu_level == 2 ? op_smalltbl_1
			  : cpu_prefs.cpu_level == 1 ? op_smalltbl_2
			  : cpu_prefs.cpu_compatible ? op_smalltbl_4
			  : op_smalltbl_3);

    printf( "Building CPU function table (%d %d).\n",
	    cpu_prefs.cpu_level, cpu_prefs.cpu_compatible );

    for (opcode = 0; opcode < 65536; opcode++)
	cpufunctbl[cft_map (opcode)] = op_illg_1;
    for (i = 0; tbl[i].handler != NULL; i++) {
	if (! tbl[i].specific)
	    cpufunctbl[cft_map (tbl[i].opcode)] = tbl[i].handler;
    }
    for (opcode = 0; opcode < 65536; opcode++) {
	cpuop_func *f;

	if (table68k[opcode].mnemo == i_ILLG || table68k[opcode].clev > cpu_prefs.cpu_level)
	    continue;

	if (table68k[opcode].handler != -1) {
	    f = cpufunctbl[cft_map (table68k[opcode].handler)];
	    if (f == op_illg_1)
		abort();
	    cpufunctbl[cft_map (opcode)] = f;
	}
    }
    for (i = 0; tbl[i].handler != NULL; i++) {
	if (tbl[i].specific)
	    cpufunctbl[cft_map (tbl[i].opcode)] = tbl[i].handler;
    }
}

unsigned long cycles_mask, cycles_val;

static void update_68k_cycles (void)
{
    cycles_mask = 0;
    cycles_val = cpu_prefs.m68k_speed;
    if (cpu_prefs.m68k_speed < 1) {
	cycles_mask = 0xFFFFFFFF;
	cycles_val = 0;
    }
}

void check_prefs_changed_cpu (void)
{
  /* BUG: this should be implemented */
#if 0
    if (cpu_prefs.cpu_level != changed_prefs.cpu_level
	|| cpu_prefs.cpu_compatible != changed_prefs.cpu_compatible) {
	cpu_prefs.cpu_level = changed_prefs.cpu_level;
	cpu_prefs.cpu_compatible = changed_prefs.cpu_compatible;
	build_cpufunctbl ();
    }
    if (cpu_prefs.m68k_speed != changed_prefs.m68k_speed) {
	cpu_prefs.m68k_speed = changed_prefs.m68k_speed;
	reset_frame_rate_hack ();
	update_68k_cycles ();
    }
#endif
}

void init_m68k (void)
{
    int i;

    update_68k_cycles ();

    for (i = 0 ; i < 256 ; i++) {
	int j;
	for (j = 0 ; j < 8 ; j++) {
		if (i & (1 << j)) break;
	}
	movem_index1[i] = j;
	movem_index2[i] = 7-j;
	movem_next[i] = i & (~(1 << j));
    }
    for (i = 0 ; i < 256 ; i++) {
	int j;
	for (j = 7 ; j >= 0 ; j--) {
		if (i & (1 << j)) break;
	}
	fpp_movem_index1[i] = j;
	fpp_movem_index2[i] = 7-j;
	fpp_movem_next[i] = i & (~(1 << j));
    }
#if COUNT_INSTRS
    {
	FILE *f = fopen (icountfilename (), "r");
	memset (instrcount, 0, sizeof instrcount);
	if (f) {
	    UInt32 opcode, count, total;
	    char name[20];
	    printf ("Reading instruction count file...\n");
	    fscanf (f, "Total: %lu\n", &total);
	    while (fscanf (f, "%lx: %lu %s\n", &opcode, &count, name) == 3) {
		instrcount[opcode] = count;
	    }
	    fclose(f);
	}
    }
#endif
    printf ("Building CPU table... ");
    read_table68k ();
    do_merges ();

    printf ("%d CPU functions\n", nr_cpuop_funcs);

    build_cpufunctbl ();
}

struct regstruct regs, lastint_regs;
static struct regstruct regs_backup[16];
static int backup_pointer = 0;
static long int m68kpc_offset;
int lastint_no;

#define get_ibyte_1(o) get_byte(regs.pc + (regs.pc_p - regs.pc_oldp) + (o) + 1)
#define get_iword_1(o) get_word(regs.pc + (regs.pc_p - regs.pc_oldp) + (o))
#define get_ilong_1(o) get_long(regs.pc + (regs.pc_p - regs.pc_oldp) + (o))

SInt32 ShowEA (int reg, amodes mode, wordsizes size, char *buf)
{
    UInt16 dp;
    SInt8 disp8;
    SInt16 disp16;
    int r;
    UInt32 dispreg;
    Ptr32 addr;
    SInt32 offset = 0;
    char buffer[80];

    switch (mode){
     case Dreg:
	sprintf (buffer,"D%d", reg);
	break;
     case Areg:
	sprintf (buffer,"A%d", reg);
	break;
     case Aind:
	sprintf (buffer,"(A%d)", reg);
	break;
     case Aipi:
	sprintf (buffer,"(A%d)+", reg);
	break;
     case Apdi:
	sprintf (buffer,"-(A%d)", reg);
	break;
     case Ad16:
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr = m68k_areg(regs,reg) + (SInt16)disp16;
	sprintf (buffer,"(A%d,$%04x) == $%08lx", reg, disp16 & 0xffff,
					(long unsigned int)addr);
	break;
     case Ad8r:
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (SInt32)(SInt16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    SInt32 outer = 0, disp = 0;
	    SInt32 base = m68k_areg(regs,reg);
	    char name[10];
	    sprintf (name,"A%d, ",reg);
	    if (dp & 0x80) { base = 0; name[0] = 0; }
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (SInt32)(SInt16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (SInt32)(SInt16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	    sprintf (buffer,"(%s%c%d.%c*%d+%ld)+%ld == $%08lx", name,
		    dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
		    1 << ((dp >> 9) & 3),
		    disp,outer,
		    (long unsigned int)addr);
	} else {
	  addr = m68k_areg(regs,reg) + (SInt32)((SInt8)disp8) + dispreg;
	  sprintf (buffer,"(A%d, %c%d.%c*%d, $%02x) == $%08lx", reg,
	       dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
	       1 << ((dp >> 9) & 3), disp8,
	       (long unsigned int)addr);
	}
	break;
     case PC16:
	addr = m68k_getpc () + m68kpc_offset;
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr += (SInt16)disp16;
	sprintf (buffer,"(PC,$%04x) == $%08lx", disp16 & 0xffff,(long unsigned int)addr);
	break;
     case PC8r:
	addr = m68k_getpc () + m68kpc_offset;
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (SInt32)(SInt16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    SInt32 outer = 0,disp = 0;
	    SInt32 base = addr;
	    char name[10];
	    sprintf (name,"PC, ");
	    if (dp & 0x80) { base = 0; name[0] = 0; }
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (SInt32)(SInt16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (SInt32)(SInt16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	    sprintf (buffer,"(%s%c%d.%c*%d+%ld)+%ld == $%08lx", name,
		    dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
		    1 << ((dp >> 9) & 3),
		    disp,outer,
		    (long unsigned int)addr);
	} else {
	  addr += (SInt32)((SInt8)disp8) + dispreg;
	  sprintf (buffer,"(PC, %c%d.%c*%d, $%02x) == $%08lx", dp & 0x8000 ? 'A' : 'D',
		(int)r, dp & 0x800 ? 'L' : 'W',  1 << ((dp >> 9) & 3),
		disp8, (long unsigned int)addr);
	}
	break;
     case absw:
	sprintf (buffer,"$%08lx", (long unsigned int)(SInt32)(SInt16)get_iword_1 (m68kpc_offset));
	m68kpc_offset += 2;
	break;
     case absl:
	sprintf (buffer,"$%08lx", (long unsigned int)get_ilong_1 (m68kpc_offset));
	m68kpc_offset += 4;
	break;
     case imm:
	switch (size){
	 case sz_byte:
	    sprintf (buffer,"#$%02x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xff));
	    m68kpc_offset += 2;
	    break;
	 case sz_word:
	    sprintf (buffer,"#$%04x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xffff));
	    m68kpc_offset += 2;
	    break;
	 case sz_long:
	    sprintf (buffer,"#$%08lx", (long unsigned int)(get_ilong_1 (m68kpc_offset)));
	    m68kpc_offset += 4;
	    break;
	 default:
	    break;
	}
	break;
     case imm0:
	offset = (SInt32)(SInt8)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	sprintf (buffer,"#$%02x", (unsigned int)(offset & 0xff));
	break;
     case imm1:
	offset = (SInt32)(SInt16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	sprintf (buffer,"#$%04x", (unsigned int)(offset & 0xffff));
	break;
     case imm2:
	offset = (SInt32)get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	sprintf (buffer,"#$%08lx", (long unsigned int)offset);
	break;
     case immi:
	offset = (SInt32)(SInt8)(reg & 0xff);
	sprintf (buffer,"#$%08lx", (long unsigned int)offset);
	break;
     default:
	break;
    }
    if (buf == 0)
	printf ("%s", buffer);
    else
	strcat (buf, buffer);
    return offset;
}

/* The plan is that this will take over the job of exception 3 handling -
 * the CPU emulation functions will just do a longjmp to m68k_go whenever
 * they hit an odd address. */
static int verify_ea (int reg, amodes mode, wordsizes size, UInt32 *val)
{
    UInt16 dp;
    SInt8 disp8;
    SInt16 disp16;
    int r;
    UInt32 dispreg;
    Ptr32 addr;
    SInt32 offset = 0;

    switch (mode){
     case Dreg:
	*val = m68k_dreg (regs, reg);
	return 1;
     case Areg:
	*val = m68k_areg (regs, reg);
	return 1;

     case Aind:
     case Aipi:
	addr = m68k_areg (regs, reg);
	break;
     case Apdi:
	addr = m68k_areg (regs, reg);
	break;
     case Ad16:
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr = m68k_areg(regs,reg) + (SInt16)disp16;
	break;
     case Ad8r:
	addr = m68k_areg (regs, reg);
     d8r_common:
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (SInt32)(SInt16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    SInt32 outer = 0, disp = 0;
	    SInt32 base = addr;
	    if (dp & 0x80) base = 0;
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (SInt32)(SInt16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (SInt32)(SInt16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	} else {
	  addr += (SInt32)((SInt8)disp8) + dispreg;
	}
	break;
     case PC16:
	addr = m68k_getpc () + m68kpc_offset;
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr += (SInt16)disp16;
	break;
     case PC8r:
	addr = m68k_getpc () + m68kpc_offset;
	goto d8r_common;
     case absw:
	addr = (SInt32)(SInt16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	break;
     case absl:
	addr = get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	break;
     case imm:
	switch (size){
	 case sz_byte:
	    *val = get_iword_1 (m68kpc_offset) & 0xff;
	    m68kpc_offset += 2;
	    break;
	 case sz_word:
	    *val = get_iword_1 (m68kpc_offset) & 0xffff;
	    m68kpc_offset += 2;
	    break;
	 case sz_long:
	    *val = get_ilong_1 (m68kpc_offset);
	    m68kpc_offset += 4;
	    break;
	 default:
	    break;
	}
	return 1;
     case imm0:
	*val = (SInt32)(SInt8)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	return 1;
     case imm1:
	*val = (SInt32)(SInt16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	return 1;
     case imm2:
	*val = get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	return 1;
     case immi:
	*val = (SInt32)(SInt8)(reg & 0xff);
	return 1;
     default:
	addr = 0;
	break;
    }
    if ((addr & 1) == 0)
	return 1;

    last_addr_for_exception_3 = m68k_getpc () + m68kpc_offset;
    last_fault_for_exception_3 = addr;
    return 0;
}

void MakeSR (void)
{
#if 0
    assert((regs.t1 & 1) == regs.t1);
    assert((regs.t0 & 1) == regs.t0);
    assert((regs.s & 1) == regs.s);
    assert((regs.m & 1) == regs.m);
    assert((XFLG & 1) == XFLG);
    assert((NFLG & 1) == NFLG);
    assert((ZFLG & 1) == ZFLG);
    assert((VFLG & 1) == VFLG);
    assert((CFLG & 1) == CFLG);
#endif
    regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
	       | (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
	       | (XFLG << 4) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1)
	       | CFLG);
}

void MakeFromSR (void)
{
    int oldm = regs.m;
    int olds = regs.s;

    regs.t1 = (regs.sr >> 15) & 1;
    regs.t0 = (regs.sr >> 14) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.m = (regs.sr >> 12) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    XFLG = (regs.sr >> 4) & 1;
    NFLG = (regs.sr >> 3) & 1;
    ZFLG = (regs.sr >> 2) & 1;
    VFLG = (regs.sr >> 1) & 1;
    CFLG = regs.sr & 1;
    if (cpu_prefs.cpu_level >= 2) {
	if (olds != regs.s) {
	    if (olds) {
		if (oldm)
		    regs.msp = m68k_areg(regs, 7);
		else
		    regs.isp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.usp;
	    } else {
		regs.usp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
	    }
	} else if (olds && oldm != regs.m) {
	    if (oldm) {
		regs.msp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.isp;
	    } else {
		regs.isp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.msp;
	    }
	}
    } else {
	if (olds != regs.s) {
	    if (olds) {
		regs.isp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.usp;
	    } else {
		regs.usp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.isp;
	    }
	}
    }

    regs.spcflags |= SPCFLAG_INT;
    if (regs.t1 || regs.t0)
	regs.spcflags |= SPCFLAG_TRACE;
    else
	regs.spcflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
}

void Exception(int nr, Ptr32 oldpc)
{
    compiler_flush_jsr_stack();
    MakeSR();
    if (!regs.s) {
	regs.usp = m68k_areg(regs, 7);
	if (cpu_prefs.cpu_level >= 2)
	    m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
	else
	    m68k_areg(regs, 7) = regs.isp;
	regs.s = 1;
    }
    if (cpu_prefs.cpu_level > 0) {
	if (nr == 2 || nr == 3) {
	    int i;
	    /* @@@ this is probably wrong (?) */
	    for (i = 0 ; i < 12 ; i++) {
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), 0);
	    }
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0xa000 + nr * 4);
	} else if (nr ==5 || nr == 6 || nr == 7 || nr == 9) {
	    m68k_areg(regs, 7) -= 4;
	    put_long (m68k_areg(regs, 7), oldpc);
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0x2000 + nr * 4);
	} else if (regs.m && nr >= 24 && nr < 32) {
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), nr * 4);
	    m68k_areg(regs, 7) -= 4;
	    put_long (m68k_areg(regs, 7), m68k_getpc ());
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), regs.sr);
	    regs.sr |= (1 << 13);
	    regs.msp = m68k_areg(regs, 7);
	    m68k_areg(regs, 7) = regs.isp;
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0x1000 + nr * 4);
	} else {
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), nr * 4);
	}
    } else {
	if (nr == 2 || nr == 3) {
	    m68k_areg(regs, 7) -= 12;
	    /* ??????? */
	    if (nr == 3) {
		put_long (m68k_areg(regs, 7), last_fault_for_exception_3);
		put_word (m68k_areg(regs, 7)+4, last_op_for_exception_3);
		put_long (m68k_areg(regs, 7)+8, last_addr_for_exception_3);
	    }
	    printf ("Exception!\n");
	    goto kludge_me_do;
	}
    }
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), m68k_getpc ());
kludge_me_do:
    m68k_areg(regs, 7) -= 2;
    put_word (m68k_areg(regs, 7), regs.sr);
    m68k_setpc (get_long (regs.vbr + 4*nr));
    fill_prefetch_0 ();
    regs.t1 = regs.t0 = regs.m = 0;
    regs.spcflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
}

static void Interrupt(int nr)
{
    assert(nr < 8 && nr >= 0);
    lastint_regs = regs;
    lastint_no = nr;
    Exception(nr+24, 0);

    regs.intmask = nr;
    regs.spcflags |= SPCFLAG_INT;
}

static int caar, cacr;

void m68k_move2c (int regno, UInt32 *regp)
{
    if (cpu_prefs.cpu_level == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7B);
    else
	switch (regno) {
	 case 0: regs.sfc = *regp & 7; break;
	 case 1: regs.dfc = *regp & 7; break;
	 case 2: cacr = *regp & 0x3; break;	/* ignore C and CE */
	 case 0x800: regs.usp = *regp; break;
	 case 0x801: regs.vbr = *regp; break;
	 case 0x802: caar = *regp &0xfc; break;
	 case 0x803: regs.msp = *regp; if (regs.m == 1) m68k_areg(regs, 7) = regs.msp; break;
	 case 0x804: regs.isp = *regp; if (regs.m == 0) m68k_areg(regs, 7) = regs.isp; break;
	 default:
	    op_illg (0x4E7B);
	    break;
	}
}

void m68k_movec2 (int regno, UInt32 *regp)
{
    if (cpu_prefs.cpu_level == 1 && (regno & 0x7FF) > 1)
	op_illg (0x4E7A);
    else
	switch (regno) {
	 case 0: *regp = regs.sfc; break;
	 case 1: *regp = regs.dfc; break;
	 case 2: *regp = cacr; break;
	 case 0x800: *regp = regs.usp; break;
	 case 0x801: *regp = regs.vbr; break;
	 case 0x802: *regp = caar; break;
	 case 0x803: *regp = regs.m == 1 ? m68k_areg(regs, 7) : regs.msp; break;
	 case 0x804: *regp = regs.m == 0 ? m68k_areg(regs, 7) : regs.isp; break;
	 default:
	    op_illg (0x4E7A);
	    break;
	}
}

static INLINE int
div_unsigned(UInt32 src_hi, UInt32 src_lo, UInt32 div, UInt32 *quot, UInt32 *rem)
{
	UInt32 q = 0, cbit = 0;
	int i;

	if (div <= src_hi) {
	    return 1;
	}
	for (i = 0 ; i < 32 ; i++) {
		cbit = src_hi & 0x80000000ul;
		src_hi <<= 1;
		if (src_lo & 0x80000000ul) src_hi++;
		src_lo <<= 1;
		q = q << 1;
		if (cbit || div <= src_hi) {
			q |= 1;
			src_hi -= div;
		}
	}
	*quot = q;
	*rem = src_hi;
	return 0;
}

void m68k_divl (UInt32 opcode, UInt32 src, UInt16 extra, Ptr32 oldpc)
{
#if HAVE_INT64
    if (src == 0) {
	Exception (5, oldpc);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	SInt64 a = (SInt64)(SInt32)m68k_dreg(regs, (extra >> 12) & 7);
	SInt64 quot, rem;

	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (SInt64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (SInt64)(SInt32)src;
	quot = a / (SInt64)(SInt32)src;
	if ((quot & UVAL64(0xffffffff80000000)) != 0
	    && (quot & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    VFLG = NFLG = 1;
	    CFLG = 0;
	} else {
	    if (((SInt32)rem < 0) != ((SInt64)a < 0)) rem = -rem;
	    VFLG = CFLG = 0;
	    ZFLG = ((SInt32)quot) == 0;
	    NFLG = ((SInt32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	UInt64 a = (UInt64)(UInt32)m68k_dreg(regs, (extra >> 12) & 7);
	UInt64 quot, rem;

	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (UInt64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (UInt64)src;
	quot = a / (UInt64)src;
	if (quot > 0xffffffffu) {
	    VFLG = NFLG = 1;
	    CFLG = 0;
	} else {
	    VFLG = CFLG = 0;
	    ZFLG = ((SInt32)quot) == 0;
	    NFLG = ((SInt32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#else
    if (src == 0) {
	Exception (5, oldpc);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	SInt32 lo = (SInt32)m68k_dreg(regs, (extra >> 12) & 7);
	SInt32 hi = lo < 0 ? -1 : 0;
	SInt32 save_high;
	UInt32 quot, rem;
	UInt32 sign;

	if (extra & 0x400) {
	    hi = (SInt32)m68k_dreg(regs, extra & 7);
	}
	save_high = hi;
	sign = (hi ^ src);
	if (hi < 0) {
	    hi = ~hi;
	    lo = -lo;
	    if (lo == 0) hi++;
	}
	if ((SInt32)src < 0) src = -src;
	if (div_unsigned(hi, lo, src, &quot, &rem) ||
	    (sign & 0x80000000) ? quot > 0x80000000 : quot > 0x7fffffff) {
	    VFLG = NFLG = 1;
	    CFLG = 0;
	} else {
	    if (sign & 0x80000000) quot = -quot;
	    if (((SInt32)rem < 0) != (save_high < 0)) rem = -rem;
	    VFLG = CFLG = 0;
	    ZFLG = ((SInt32)quot) == 0;
	    NFLG = ((SInt32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	UInt32 lo = (UInt32)m68k_dreg(regs, (extra >> 12) & 7);
	UInt32 hi = 0;
	UInt32 quot, rem;

	if (extra & 0x400) {
	    hi = (UInt32)m68k_dreg(regs, extra & 7);
	}
	if (div_unsigned(hi, lo, src, &quot, &rem)) {
	    VFLG = NFLG = 1;
	    CFLG = 0;
	} else {
	    VFLG = CFLG = 0;
	    ZFLG = ((SInt32)quot) == 0;
	    NFLG = ((SInt32)quot) < 0;
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#endif
}

static INLINE void
mul_unsigned(UInt32 src1, UInt32 src2, UInt32 *dst_hi, UInt32 *dst_lo)
{
	UInt32 r0 = (src1 & 0xffff) * (src2 & 0xffff);
	UInt32 r1 = ((src1 >> 16) & 0xffff) * (src2 & 0xffff);
	UInt32 r2 = (src1 & 0xffff) * ((src2 >> 16) & 0xffff);
	UInt32 r3 = ((src1 >> 16) & 0xffff) * ((src2 >> 16) & 0xffff);
	UInt32 lo;

	lo = r0 + ((r1 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r0 = lo;
	lo = r0 + ((r2 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r3 += ((r1 >> 16) & 0xffff) + ((r2 >> 16) & 0xffff);
	*dst_lo = lo;
	*dst_hi = r3;
}

void m68k_mull (UInt32 opcode, UInt32 src, UInt16 extra)
{
#if HAVE_INT64
    if (extra & 0x800) {
	/* signed variant */
	SInt64 a = (SInt64)(SInt32)m68k_dreg(regs, (extra >> 12) & 7);

	a *= (SInt64)(SInt32)src;
	VFLG = CFLG = 0;
	ZFLG = a == 0;
	NFLG = a < 0;
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = a >> 32;
	else if ((a & UVAL64(0xffffffff80000000)) != 0
		 && (a & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    VFLG = 1;
	}
	m68k_dreg(regs, (extra >> 12) & 7) = (UInt32)a;
    } else {
	/* unsigned */
	UInt64 a = (UInt64)(UInt32)m68k_dreg(regs, (extra >> 12) & 7);

	a *= (UInt64)src;
	VFLG = CFLG = 0;
	ZFLG = a == 0;
	NFLG = ((SInt64)a) < 0;
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = a >> 32;
	else if ((a & UVAL64(0xffffffff00000000)) != 0) {
	    VFLG = 1;
	}
	m68k_dreg(regs, (extra >> 12) & 7) = (UInt32)a;
    }
#else
    if (extra & 0x800) {
	/* signed variant */
	SInt32 src1,src2;
	UInt32 dst_lo,dst_hi;
	UInt32 sign;

	src1 = (SInt32)src;
	src2 = (SInt32)m68k_dreg(regs, (extra >> 12) & 7);
	sign = (src1 ^ src2);
	if (src1 < 0) src1 = -src1;
	if (src2 < 0) src2 = -src2;
	mul_unsigned((UInt32)src1,(UInt32)src2,&dst_hi,&dst_lo);
	if (sign & 0x80000000) {
		dst_hi = ~dst_hi;
		dst_lo = -dst_lo;
		if (dst_lo == 0) dst_hi++;
	}
	VFLG = CFLG = 0;
	ZFLG = dst_hi == 0 && dst_lo == 0;
	NFLG = ((SInt32)dst_hi) < 0;
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if ((dst_hi != 0 || (dst_lo & 0x80000000) != 0)
		 && ((dst_hi & 0xffffffff) != 0xffffffff
		     || (dst_lo & 0x80000000) != 0x80000000))
	{
	    VFLG = 1;
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    } else {
	/* unsigned */
	UInt32 dst_lo,dst_hi;

	mul_unsigned(src,(UInt32)m68k_dreg(regs, (extra >> 12) & 7),&dst_hi,&dst_lo);

	VFLG = CFLG = 0;
	ZFLG = dst_hi == 0 && dst_lo == 0;
	NFLG = ((SInt32)dst_hi) < 0;
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if (dst_hi != 0) {
	    VFLG = 1;
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    }
#endif
}
static char* ccnames[] =
{ "T ","F ","HI","LS","CC","CS","NE","EQ",
  "VC","VS","PL","MI","GE","LT","GT","LE" };

void m68k_reset (void)
{
    m68k_areg (regs, 7) = get_long (0x00f80000);
    m68k_setpc (get_long (0x00f80004));
    fill_prefetch_0 ();
    regs.kick_mask = 0xF80000;
    regs.s = 1;
    regs.m = 0;
    regs.stopped = 0;
    regs.t1 = 0;
    regs.t0 = 0;
    ZFLG = CFLG = NFLG = VFLG = 0;
    regs.spcflags = 0;
    regs.intmask = 7;
    regs.vbr = regs.sfc = regs.dfc = 0;
    regs.fpcr = regs.fpsr = regs.fpiar = 0;
}

unsigned long op_illg (UInt32 opcode)
{
    Ptr32 pc = m68k_getpc ();
    
    compiler_flush_jsr_stack ();

    if ((opcode & 0xF000) == 0xA000 && (pc & 0xF80000) == 0xF00000) {
	/* Calltrap. */
	m68k_incpc(2);
	call_calltrap (opcode & 0xFFF);
	fill_prefetch_0 ();
	return 4;
    }

    if ((opcode & 0xF000) == 0xF000) {
	Exception(0xB,0);
	return 4;
    }
    if ((opcode & 0xF000) == 0xA000) {
	Exception(0xA,0);
	return 4;
    }
    printf ("Illegal instruction: %04x at %08lx\n", opcode, pc);
    Exception (4,0);
    return 4;
}

void mmu_op(UInt32 opcode, UInt16 extra)
{
    if ((extra & 0xB000) == 0) { /* PMOVE instruction */

    } else if ((extra & 0xF000) == 0x2000) { /* PLOAD instruction */
    } else if ((extra & 0xF000) == 0x8000) { /* PTEST instruction */
    } else
	op_illg (opcode);
}

static int n_insns = 0, n_spcinsns = 0;

static Ptr32 last_trace_ad = 0;

static INLINE void do_trace (void)
{
    if (regs.spcflags & SPCFLAG_TRACE) {		/* 6 */
	if (regs.t0) {
	    UInt16 opcode;
	    /* should also include TRAP, CHK, SR modification FPcc */
	    /* probably never used so why bother */
	    /* We can afford this to be inefficient... */
	    m68k_setpc (m68k_getpc ());
	    fill_prefetch_0 ();
	    opcode = get_word (regs.pc);
	    if (opcode == 0x4e72 		/* RTE */
		|| opcode == 0x4e74 		/* RTD */
		|| opcode == 0x4e75 		/* RTS */
		|| opcode == 0x4e77 		/* RTR */
		|| opcode == 0x4e76 		/* TRAPV */
		|| (opcode & 0xffc0) == 0x4e80 	/* JSR */
		|| (opcode & 0xffc0) == 0x4ec0 	/* JMP */
		|| (opcode & 0xff00) == 0x6100  /* BSR */
		|| ((opcode & 0xf000) == 0x6000	/* Bcc */
		    && cctrue((opcode >> 8) & 0xf))
		|| ((opcode & 0xf0f0) == 0x5050 /* DBcc */
		    && !cctrue((opcode >> 8) & 0xf)
		    && (SInt16)m68k_dreg(regs, opcode & 7) != 0))
	    {
		last_trace_ad = m68k_getpc ();
		regs.spcflags &= ~SPCFLAG_TRACE;
		regs.spcflags |= SPCFLAG_DOTRACE;
	    }
	} else if (regs.t1) {
	    last_trace_ad = m68k_getpc ();
	    regs.spcflags &= ~SPCFLAG_TRACE;
	    regs.spcflags |= SPCFLAG_DOTRACE;
	}
    }
}

static int do_specialties (void)
{
  /* BUG: it hasn't been verified weither this can be deleted or not */
  printf( "do_specialties called, this shouldn't happen\n" );
#if 0
    /*n_spcinsns++;*/
    while (regs.spcflags & SPCFLAG_BLTNASTY) {
	do_cycles (4);
	if (regs.spcflags & SPCFLAG_DISK)
	    do_disk ();
    }
    run_compiled_code();
    if (regs.spcflags & SPCFLAG_DOTRACE) {
	Exception (9,last_trace_ad);
    }
    while (regs.spcflags & SPCFLAG_STOP) {
	do_cycles (4);
	if (regs.spcflags & SPCFLAG_DISK)
	    do_disk ();
	if (regs.spcflags & (SPCFLAG_INT | SPCFLAG_DOINT)){
	    int intr = intlev ();
	    regs.spcflags &= ~(SPCFLAG_INT | SPCFLAG_DOINT);
	    if (intr != -1 && intr > regs.intmask) {
		Interrupt (intr);
		regs.stopped = 0;
		regs.spcflags &= ~SPCFLAG_STOP;
	    }
	}
    }
    do_trace ();

    if (regs.spcflags & SPCFLAG_DISK)
	do_disk ();

    if (regs.spcflags & SPCFLAG_DOINT) {
	int intr = intlev ();
	regs.spcflags &= ~SPCFLAG_DOINT;
	if (intr != -1 && intr > regs.intmask) {
	    Interrupt (intr);
	    regs.stopped = 0;
	}
    }
    if (regs.spcflags & SPCFLAG_INT) {
	regs.spcflags &= ~SPCFLAG_INT;
	regs.spcflags |= SPCFLAG_DOINT;
    }
    if (regs.spcflags & (SPCFLAG_BRK | SPCFLAG_MODE_CHANGE)) {
	regs.spcflags &= ~(SPCFLAG_BRK | SPCFLAG_MODE_CHANGE);
	return 1;
    }
#endif
    return 0;
}

static void m68k_run_1 (void)
{
    for (;;) {
	int cycles;
	UInt32 opcode = GET_OPCODE;
#if 0
	if (get_ilong (0) != do_get_mem_long (&regs.prefetch)) {
	    debugging = 1;
	    return;
	}
#endif
	/* assert (!regs.stopped && !(regs.spcflags & SPCFLAG_STOP)); */
/*	regs_backup[backup_pointer = (backup_pointer + 1) % 16] = regs;*/
#if COUNT_INSTRS == 2
	if (table68k[cft_map (opcode)].handler != -1)
	    instrcount[table68k[cft_map (opcode)].handler]++;
#elif COUNT_INSTRS == 1
	instrcount[opcode]++;
#endif
#if defined(X86_ASSEMBLY)
	__asm__ __volatile__("\tcall *%%ebx"
			     : "=&a" (cycles) : "b" (cpufunctbl[opcode]), "0" (opcode)
			     : "%edx", "%ecx",
			     "%esi", "%edi", "%ebp", "memory", "cc");
#else
	cycles = (*cpufunctbl[opcode])(opcode);
#endif
	/*n_insns++;*/
	cycles &= cycles_mask;
	cycles |= cycles_val;
	do_cycles (cycles);
	if (regs.spcflags) {
	    if (do_specialties ())
		return;
	}
    }
}

#ifdef X86_ASSEMBLY
static INLINE void m68k_run1 (void)
{
    /* Work around compiler bug: GCC doesn't push %ebp in m68k_run_1. */
    __asm__ __volatile__ ("pushl %%ebp\n\tcall *%0\n\tpopl %%ebp" : : "r" (m68k_run_1) : "%eax", "%edx", "%ecx", "memory", "cc");
}
#else
#define m68k_run1 m68k_run_1
#endif

int in_m68k_go = 0;

void m68k_go (int may_quit)
{
    if (in_m68k_go || !may_quit) {
	printf ("Bug! m68k_go is not reentrant.\n");
	abort ();
    }

    reset_frame_rate_hack ();
    update_68k_cycles ();

    in_m68k_go++;
    for (;;) {
	if (quit_program > 0) {
	    if (quit_program == 1)
		break;
	    quit_program = 0;
	    m68k_reset ();
	    reset_all_systems ();
	    customreset ();
	}
	if (debugging)
	    debug ();
	m68k_run1 ();
    }
    in_m68k_go--;
}

static void m68k_verify (Ptr32 addr, Ptr32 *nextpc)
{
    UInt32 opcode, val;
    struct instr *dp;

    opcode = get_iword_1(0);
    last_op_for_exception_3 = opcode;
    m68kpc_offset = 2;

    if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
	opcode = 0x4AFC;
    }
    dp = table68k + opcode;

    if (dp->suse) {
	if (!verify_ea (dp->sreg, dp->smode, dp->size, &val)) {
	    Exception (3, 0);
	    return;
	}
    }
    if (dp->duse) {
	if (!verify_ea (dp->dreg, dp->dmode, dp->size, &val)) {
	    Exception (3, 0);
	    return;
	}
    }
}

void m68k_disasm (Ptr32 addr, Ptr32 *nextpc, int cnt)
{
    Ptr32 newpc = 0;
    m68kpc_offset = addr - m68k_getpc ();
    while (cnt-- > 0) {
	char instrname[20],*ccpt;
	int opwords;
	UInt32 opcode;
	struct mnemolookup *lookup;
	struct instr *dp;
	printf ("%08lx: ", m68k_getpc () + m68kpc_offset);
	for (opwords = 0; opwords < 5; opwords++){
	    printf ("%04x ", get_iword_1 (m68kpc_offset + opwords*2));
	}
	opcode = get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
	    opcode = 0x4AFC;
	}
	dp = table68k + opcode;
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
	    ;

	strcpy (instrname, lookup->name);
	ccpt = strstr (instrname, "cc");
	if (ccpt != 0) {
	    strncpy (ccpt, ccnames[dp->cc], 2);
	}
	printf ("%s", instrname);
	switch (dp->size){
	 case sz_byte: printf (".B "); break;
	 case sz_word: printf (".W "); break;
	 case sz_long: printf (".L "); break;
	 default: printf ("   "); break;
	}

	if (dp->suse) {
	    newpc = m68k_getpc () + m68kpc_offset;
	    newpc += ShowEA (dp->sreg, dp->smode, dp->size, 0);
	}
	if (dp->suse && dp->duse)
	    printf (",");
	if (dp->duse) {
	    newpc = m68k_getpc () + m68kpc_offset;
	    newpc += ShowEA (dp->dreg, dp->dmode, dp->size, 0);
	}
	if (ccpt != 0) {
	    if (cctrue(dp->cc))
		printf (" == %08lx (TRUE)", newpc);
	    else
		printf (" == %08lx (FALSE)", newpc);
	} else if ((opcode & 0xff00) == 0x6100) /* BSR */
	    printf (" == %08lx", newpc);
	printf ("\n");
    }
    if (nextpc)
	*nextpc = m68k_getpc () + m68kpc_offset;
}

void m68k_dumpstate (Ptr32 *nextpc)
{
    int i;
    for (i = 0; i < 8; i++){
	printf ("D%d: %08lx ", i, m68k_dreg(regs, i));
	if ((i & 3) == 3) printf ("\n");
    }
    for (i = 0; i < 8; i++){
	printf ("A%d: %08lx ", i, m68k_areg(regs, i));
	if ((i & 3) == 3) printf ("\n");
    }
    if (regs.s == 0) regs.usp = m68k_areg(regs, 7);
    if (regs.s && regs.m) regs.msp = m68k_areg(regs, 7);
    if (regs.s && regs.m == 0) regs.isp = m68k_areg(regs, 7);
    printf ("USP=%08lx ISP=%08lx MSP=%08lx VBR=%08lx\n",
	    regs.usp,regs.isp,regs.msp,regs.vbr);
    printf ("T=%d%d S=%d M=%d X=%d N=%d Z=%d V=%d C=%d IMASK=%d\n",
	    regs.t1, regs.t0, regs.s, regs.m,
	    XFLG, NFLG, ZFLG, VFLG, CFLG, regs.intmask);
    for (i = 0; i < 8; i++){
	printf ("FP%d: %g ", i, regs.fp[i]);
	if ((i & 3) == 3) printf ("\n");
    }
    printf ("N=%d Z=%d I=%d NAN=%d\n",
		(regs.fpsr & 0x8000000) != 0,
		(regs.fpsr & 0x4000000) != 0,
		(regs.fpsr & 0x2000000) != 0,
		(regs.fpsr & 0x1000000) != 0);
    if (cpu_prefs.cpu_compatible)
	printf ("prefetch %08lx\n", (unsigned long)do_get_mem_long(&regs.prefetch));

    m68k_disasm(m68k_getpc (), nextpc, 1);
    if (nextpc)
	printf ("next PC: %08lx\n", *nextpc);
}
