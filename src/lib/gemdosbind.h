/*
** gemdosbind.h
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

#ifndef _GEMDOSBIND_H_
#define _GEMDOSBIND_H_

/*
** GEMDOS defines and structures
*/

/* Structure used by Dfree() */
typedef struct {
    long b_free;	/* number of free clusters */
    long b_total;	/* total number of clusters */
    long b_secsiz;	/* number of bytes per sector */
    long b_clsiz;	/* number of sectors per cluster */
} _DISKINFO;

/* Structure returned by Fdatime() */
typedef struct {
  short time;
  short date;
} _DOSTIME;

/* Structure used by Cconrs */
typedef struct
{
        unsigned char maxlen;
        unsigned char actuallen;
        char    buffer[255];
} _CCONLINE;

#ifdef __TURBOC__
#define LINE _CCONLINE
#endif

/* Structure used by Fgetdta(), Fsetdta(), Fsfirst(), Fsnext() */
typedef struct _dta {
    char 	    dta_buf[21];	/* reserved */
    char            dta_attribute;	/* file attribute */
    unsigned short  dta_time;		/* file time stamp */
    unsigned short  dta_date;		/* file date stamp */
    long            dta_size;		/* file size */
    char            dta_name[14];	/* file name */
} _DTA;

/* Codes used with Fsfirst() */

#define        FA_RDONLY           0x01
#define        FA_HIDDEN           0x02
#define        FA_SYSTEM           0x04
#define        FA_LABEL            0x08
#define        FA_DIR              0x10
#define        FA_CHANGED          0x20

/* Codes used with Pexec */

#define        PE_LOADGO           0           /* load & go */
#define        PE_LOAD             3           /* just load */
#define        PE_GO               4           /* just go */
#define        PE_CBASEPAGE        5           /* just create basepage */
/* Tos 1.4: like 4, but memory ownership changed to child, and freed
   on exit
 */
#define        PE_GO_FREE          6           /* just go, then free */

typedef struct basep {
    char	*p_lowtpa;	/* pointer to self (bottom of TPA) */
    char	*p_hitpa;	/* pointer to top of TPA + 1 */
    char	*p_tbase;	/* base of text segment */
    long	p_tlen;		/* length of text segment */
    char	*p_dbase;	/* base of data segment */
    long	p_dlen;		/* length of data segment */
    char	*p_bbase;	/* base of BSS segment */
    long	p_blen;		/* length of BSS segment */
    char	*p_dta;		/* (UNOFFICIAL, DON'T USE) */
    struct basep *p_parent;	/* pointer to parent's basepage */
    char	*p_reserved;	/* reserved for future use */
    char	*p_env;		/* pointer to environment string */
#if 0
/* none of these are documented by Atari. If you try to use them under MiNT,
 * MiNT will laugh at you. So will I.
 */
    char	devx[6];	/* real handles of the standard devices */
    char	res2;		/* reserved */
    char	defdrv;		/* default drv */
#else
    char	p_junk[8];
#endif
    long	p_undef[18];	/* scratch area... don't touch */
    char	p_cmdlin[128];	/* command line image */
} BASEPAGE;

/* Alternative structure names, according to Atari Compendium */
typedef _DISKINFO DISKINFO;
typedef _DOSTIME DATETIME;
typedef _DTA DTA;



/* GEMDOS function prototypes */
short Cauxin(void);
short Cauxis(void);
short Cauxos(void);
void Cauxout(short);
long Cconin(void);
short Cconis(void);
short Cconos(void);
void Cconout(short);
void Cconrs(char *);
void Cconws(char *);
short Cnecin(void);
short Cprnos(void);
short Cprnout(short);
long Crawcin(void);
long Crawio(short);
long Dcreate(char *);
long Ddelete(char *);
long Dfree(DISKINFO *,short);
short Dgetdrv(void);
long Dgetpath(char *,short);
long Dsetdrv(short);
long Dsetpath(char *);
long Fattrib(char *,short,short);
long Fclose(short);
long Fcreate(char *,short);
long Fdatime(DATETIME *,short,short);
long Fdelete(char *);
long Fdup(short);
long Fforce(short,short);
DTA *Fgetdta(void);
long Flock(short,short,long,long);
long Fopen(char *,short);
long Fread(short,long,void *);
long Frename(short,char *,char *);
long Fseek(long,short,short);
void Fsetdta(DTA *);
short Fsfirst(char *,short);
short Fsnext(void);
long Fwrite(short,long,void *);
long Maddalt(void *,long);
void *Malloc(long);
short Mfree(void *);
short Mshrink(void *,long);
void *Mxalloc(long,short);
long Pexec(short,char *,char *,char *);
void Pterm(short);
void Pterm0(void);
void Ptermres(long,short);
void *Super(void *);
unsigned short Sversion(void);
unsigned short Tgetdate(void);
unsigned short Tgettime(void);
short Tsetdate(unsigned short);
short Tsettime(unsigned short);

#endif /* _GEMDOSBIND_H_ */
