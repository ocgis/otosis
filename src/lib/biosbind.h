/*
** biosbind.h
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

#ifndef _BIOSBIND_H_
#define _BIOSBIND_H_

/*
** BIOS defines and structures
*/

/* Device codes for Bconin(), Bconout(), Bcostat(), Bconstat() */
#define _PRT    0
#define _AUX    1
#define _CON    2
#define _MIDI   3
#define _IKBD   4
#define _RAWCON 5

/* Structure returned by Getbpb() */
typedef struct {
  short recsiz;         /* bytes per sector */
  short clsiz;          /* sectors per cluster */
  short clsizb;         /* bytes per cluster */
  short rdlen;          /* root directory size */
  short fsiz;           /* size of file allocation table */
  short fatrec;         /* startsector of second FAT */
  short datrec;         /* first data sector */
  short numcl;          /* total number of clusters */
  short bflags;         /* some flags */
} _BPB;

/* Structures used by Getmpb() */

/* Memory descriptor */
typedef struct _md {
    struct _md	*md_next;	/* next descriptor in the chain */
    long	 md_start;	/* starting address of block */
    long	 md_length;	/* length of the block */
    long	 md_owner;	/* owner's process descriptor */
} _MD;

/* Memory parameter block */
typedef struct {
    _MD *mp_free;		/* free memory chunks */
    _MD *mp_used;		/* used memory descriptors */
    _MD *mp_rover;		/* rover memory descriptor */
} _MPB;


/* BIOS function prototypes */
typedef void voidfunc(void);

long Bconin(short);
long Bconout(short);
long Bconstat(short);
long Bcostat(short);
unsigned long Drvmap(void);
Bpb *Getbpb(short);
void Getmpb(void *);
long Kbshift(short);
long Mediach(short);
long Rwabs(short,void *,short,short,short,long);
voidfunc *Setexc(short,voidfunc *);
long Tickcal(void);

#endif /* _BIOSBIND_H_ */
