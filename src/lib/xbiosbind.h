/*
** xbiosbind.h
**
** Copyright 1999 Christer Gustavsson <cg@nocrew.org>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**  
** Read the file COPYING for more information.
**
*/

#ifndef _XBIOSBIND_H_
#define _XBIOSBIND_H_

/*
** XBIOS defines and structures
*/

/* Codes used with Cursconf() */
#define CURS_HIDE   	0
#define CURS_SHOW   	1
#define CURS_BLINK  	2
#define CURS_NOBLINK    3
#define CURS_SETRATE    4
#define CURS_GETRATE    5

/* Structure returned by Iorec() */
typedef struct {
    char    *ibuf;
    short   ibufsiz;
    volatile short   ibufhd;
    volatile short   ibuftl;
    short   ibuflow;
    short   ibufhi;
} _IOREC;

/* Structure used by Initmouse() */
typedef struct {
	char	topmode;
	char	buttons;
	char	xparam;
	char	yparam;
	short	xmax;
	short	ymax;
	short	xstart;
	short	ystart;
} _PARAM;

/* Structure returned by Kbdvbase() */
typedef struct {
    void    (*midivec)	__PROTO((void));
    void    (*vkbderr)	__PROTO((void));
    void    (*vmiderr)	__PROTO((void));
    void    (*statvec)	__PROTO((void *));
    void    (*mousevec)	__PROTO((void *));
    void    (*clockvec)	__PROTO((void *));
    void    (*joyvec)	__PROTO((void *));
    long    (*midisys)	__PROTO((void));
    long    (*ikbdsys)	__PROTO((void));
    char    kbstate;
} _KBDVECS;

/* Structure returned by Keytbl() */
typedef struct {
    void *unshift;	/* pointer to unshifted keys */
    void *shift;	/* pointer to shifted keys */
    void *caps;		/* pointer to capslock keys */
} _KEYTAB;

/* Structure used by Prtblk() */
typedef struct
{
        void    *pb_scrptr;
        int     pb_offset;
        int     pb_width;
        int     pb_height;
        int     pb_left;
        int     pb_right;
        int     pb_screz;
        int     pb_prrez;
        void    *pb_colptr;
        int     pb_prtype;
        int     pb_prport;
        void    *pb_mask;
} _PBDEF;

/* Structure used by VgetRGB() and VsetRGB() */
typedef struct
{
  unsigned char reserved;
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} _RGB;

/* Structure used by Buffptr() */
typedef struct
{
  void *playptr;
  void *recordptr;
  void *reserved1;
  void *reserved2;
} _SBUFPTR;

/* Structure used by Dsp_MultBlocks() */
typedef struct
{
#define BLOCK_LONG 0
#define BLOCK_WORD 1
#define BLOCK_UBYTE 2
  short blocktype;
  long blocksize;
  void *blockaddr;
} _DSPBLOCK;

/* Structure used by Metainit() */
typedef struct
{
  unsigned long drivemap;
  char *version;
  long reserved[2];
} _METAINFO;

/* Use alternative names for structures */
typedef _RGB RGB;
typedef _PBDEF PRTBLK;
typedef _IOREC IOREC;
typedef _KBDVECS KBDVECS;
typedef _KEYTAB KEYTAB;
typedef _SBUFPTR SBUFPTR;
typedef _DSPBLOCK DSPBLOCK;
typedef _METAINFO METAINFO;


/* XBIOS function prototypes */
typedef void voidfunc(void);
typedef long longfunc(void);

long Bconmap(short);
void Bioskeys(void);
short Blitmode(short);
long Buffoper(short);
long Buffptr(SBUFPTR *);
short Cursconf(short,short);
void Dbmsg(short,short,long);
long Devconnect(short,short,short,short,short);
long DMAread(long,short,void *,short);
long DMAwrite(long,short,void *,short);
void Dosound(char *);
void Dsp_Available(long *,long *);
void Dsp_BlkBytes(unsigned char *,long,unsigned char *,long);
void Dsp_BlkHandShake(char *,long,char *,long);
void Dsp_BlkUnpacked(long *,long,long *,long);
void Dsp_BlkWords(short *,long,short *,long);
void Dsp_DoBlock(char *,long,char *,long);
void Dsp_ExecBoot(char *,long,short);
void Dsp_ExecProg(char *,long,short);
void Dsp_FlushSubroutines(void);
short Dsp_GetProgAbility(void);
short Dsp_GetWordSize(void);
short Dsp_Hf0(short);
short Dsp_Hf1(short);
short Dsp_Hf2(void);
short Dsp_Hf3(void);
char Dsp_HStat(void);
short Dsp_InqSubrAbility(short);
void Dsp_InStream(char *,long,long,long *);
void Dsp_IOStream(char *,char *,long,long,long,long *);
short Dsp_LoadProg(char *,short,char *);
short Dsp_LoadSubroutine(char *,long,short);
short Dsp_Lock(void);
long Dsp_LodToBinary(char *,char *);
void Dsp_MultBlocks(long,long,DSPBLOCK *,DSPBLOCK *);
void Dsp_OutStream(char *,long,long,long *);
void Dsp_RemoveInterrupts(short);
short Dsp_RequestUniqueAbility(void);
short Dsp_Reserve(long,long);
short Dsp_RunSubroutine(short);
void Dsp_SetVectors(voidfunc *,longfunc *);
void Dsp_TriggerHC(short);
void Dsp_Unlock(void);
long Dsptristate(short,short);
void EgetPalette(short,short,short *);
short EgetShift(void);
short EsetBank(short);
short EsetColor(short,short);
short EsetGray(short);
void EsetPalette(short,short,short *);
short EsetShift(short);
short EsetSmear(short);
short Flopfmt(void *,short *,short,short,short,short,short,long,short);
short Floprate(short,short);
short Floprd(void *,long,short,short,short,short,short);
short Flopver(void *,long,short,short,short,short,short);
short Flopwr(void *,long,short,short,short,short,short);
short Getrez(void);
long Gettime(void);
short Giaccess(short,short);
long Gpio(short,short);
void Ikbdws(short,char *);
void Initmous (short,void *,voidfunc *);
IOREC *Iorec(short);
void Jdisint(short);
void Jenabint(short);
KBDVECS *Kbdvbase(void);
short Kbrate(short,short);
KEYTAB *Keytbl(char *,char *,char *);
long Locksnd(void);
void *Logbase(void);
void Metainit(METAINFO *);
void Mfpint(short,voidfunc *);
void Midiws(short,char *);
short NVMaccess(short,short,short,char *);
void Offgibit(short);
void Ongibit(short);
void *Physbase(void);
void Protobt(void *,long,short,short);
short Prtblk(PRTBLK *);
void Puntaes(void);
long Random(void);
unsigned long Rsconf(short,short,short,short,short,short);
void Scrdmp(void);
long Setbuffer(short,void *,void *);
short Setcolor(short,short);
long Setinterrupt(short,short);
long Setmode(short);
long Setmontracks(short);
void Setpalette(short *);
short Setprt(short);
void Setscreen(void *,void *,short);
void Settime(long);
long Settracks(short,short);
long Sndstatus(short);
long Soundcmd(short,short);
void *Ssbrk(short);
long Supexec(longfunc *);
long Unlocksnd(void);
short VgetMonitor(void);
void VgetRGB(short,short,RGB *);
long VgetSize(short);
void VsetMask(long,long,short);
short VsetMode(short);
void VsetRGB(short,short,RGB *);
void VsetScreen(void *,void *,short,short);
void VsetSync(short);
void Vsync(void);
void Xbtimer(short,short,short,voidfunc *);


#endif /* _XBIOSBIND_H_ */
