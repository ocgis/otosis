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

#ifndef _MINTBIND_H_
#define _MINTBIND_H_

#include <gemdosbind.h>

/* MiNT modes for Pexec */
#  define	PE_ASYNC_LOADGO	   100	       /* load and asynchronously go */
#  define       PE_ASYNC_GO	   104	       /* asynchronously go	     */
#  define       PE_ASYNC_GO_FREE   106	       /* asynchronously go and free */
#  define       PE_OVERLAY	   200	       /* load and overlay	     */

/* ioctl's used with Fcntl */
#define FSTAT		(('F'<< 8) | 0)
#define FIONREAD	(('F'<< 8) | 1)
#define FIONWRITE	(('F'<< 8) | 2)
#define FUTIME		(('F'<< 8) | 3)
#define FTRUNCATE	(('F'<< 8) | 4)
#define TIOCGPGRP	(('T'<< 8) | 6)
#define TIOCSPGRP	(('T'<< 8) | 7)
#define TIOCFLUSH	(('T'<< 8) | 8)
#define TIOCSTOP	(('T'<< 8) | 9)
#define TIOCSTART	(('T'<< 8) | 10)
#define TIOCGXKEY	(('T'<< 8) | 13)
#define TIOCSXKEY	(('T'<< 8) | 14)

#define TIOCIBAUD	(('T'<< 8) | 18)
#define TIOCOBAUD	(('T'<< 8) | 19)
#define TIOCCBRK	(('T'<< 8) | 20)
#define TIOCSBRK	(('T'<< 8) | 21)
#define TIOCGFLAGS	(('T'<< 8) | 22)
#define TIOCSFLAGS	(('T'<< 8) | 23)
#define TIOCOUTQ	(('T'<< 8) | 24)
#define TIOCSETP	(('T'<< 8) | 25)
#define TIOCHPCL	(('T'<< 8) | 26)
#define TIOCCAR		(('T'<< 8) | 27)
#define TIOCNCAR	(('T'<< 8) | 28)
#define TIOCWONLINE	(('T'<< 8) | 29)
#define TIOCSFLAGSB	(('T'<< 8) | 30)
#define TIOCGSTATE	(('T'<< 8) | 31)
#define TIOCSSTATEB	(('T'<< 8) | 32)
#define TIOCGVMIN	(('T'<< 8) | 33)
#define TIOCSVMIN	(('T'<< 8) | 34)

/* faked by the library */
#define TIOCLBIS	(('T'<< 8) | 246)
#define TIOCLBIC	(('T'<< 8) | 247)
#define TIOCMGET	(('T'<< 8) | 248)
#define TIOCCDTR	(('T'<< 8) | 249)
#define TIOCSDTR	(('T'<< 8) | 250)
#define TIOCNOTTY	(('T'<< 8) | 251)

/* bits in longword fetched by TIOCMGET */
#define TIOCM_LE	0001 /* not supported */
#define TIOCM_DTR	0002
#define TIOCM_RTS	0004
#define TIOCM_ST	0010 /* not supported */
#define TIOCM_SR	0020 /* not supported */
#define TIOCM_CTS	0040
#define TIOCM_CAR	0100
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RNG	0200
#define TIOCM_RI	TIOCM_RNG
#define TIOCM_DSR	0400 /* not supported */

/* not yet implemented in MiNT */
#define TIOCGETD	(('T'<< 8) | 252)
#define TIOCSETD	(('T'<< 8) | 253)
#define TIOCLGET	(('T'<< 8) | 254)
#define TIOCLSET	(('T'<< 8) | 255)

#define NTTYDISC	1

/* ioctl's to act on processes */
#define PPROCADDR	(('P'<< 8) | 1)
#define PBASEADDR	(('P'<< 8) | 2)
#define PCTXTSIZE	(('P'<< 8) | 3)
#define PSETFLAGS	(('P'<< 8) | 4)
#define PGETFLAGS	(('P'<< 8) | 5)
#define PTRACESFLAGS	(('P'<< 8) | 6)
#define PTRACEGFLAGS	(('P'<< 8) | 7)
#	define	P_ENABLE	(1 << 0)	/* enable tracing */
#if 0 /* NOTYETDEFINED */
#	define	P_DOS		(1 << 1)	/* trace DOS calls - unimplemented */
#	define	P_BIOS		(1 << 2)	/* trace BIOS calls - unimplemented */
#	define	P_XBIOS		(1 << 3)	/* trace XBIOS calls - unimplemented */
#endif

#define PTRACEGO	(('P'<< 8) | 8)	/* these 4 must be together */
#define PTRACEFLOW	(('P'<< 8) | 9)
#define PTRACESTEP	(('P'<< 8) | 10)
#define PTRACE11	(('P'<< 8) | 11)
#define PLOADINFO	(('P'<< 8) | 12)
#define	PFSTAT		(('P'<< 8) | 13)

struct __ploadinfo {
	/* passed */
	short fnamelen;
	/* returned */
	char *cmdlin, *fname;
};

/* shared memory ioctl's */
#define SHMGETBLK	(('M'<< 8) | 0)
#define SHMSETBLK	(('M'<< 8) | 1)

/* cursor control ioctl's */
#define TCURSOFF	(('c'<< 8) | 0)
#define TCURSON		(('c'<< 8) | 1)
#define TCURSBLINK	(('c'<< 8) | 2)
#define TCURSSTEADY	(('c'<< 8) | 3)
#define TCURSSRATE	(('c'<< 8) | 4)
#define TCURSGRATE	(('c'<< 8) | 5)

/* Socket ioctls: these require MiNT-Net 3.0 (or later) */

/* socket-level I/O control calls */
#define SIOCGLOWAT	(('S' << 8) | 1)
#define SIOCSLOWAT	(('S' << 8) | 2)
#define SIOCGHIWAT	(('S' << 8) | 3)
#define SIOCSHIWAT	(('S' << 8) | 4)
#define SIOCSPGRP	(('S' << 8) | 5)
#define SIOCGPGRP	(('S' << 8) | 6)
#define SIOCATMARK	(('S' << 8) | 7)

/* socket configuration controls */
#define SIOCGIFCONF	(('S' << 8) | 12)	/* get iface list */
#define SIOCGIFFLAGS	(('S' << 8) | 13)	/* get flags */
#define SIOCSIFFLAGS	(('S' << 8) | 14)	/* set flags */
#define SIOCGIFADDR	(('S' << 8) | 15)	/* get iface address */
#define SIOCSIFADDR	(('S' << 8) | 16)	/* set iface address */
#define SIOCGIFDSTADDR	(('S' << 8) | 17)	/* get iface remote address */
#define SIOCSIFDSTADDR	(('S' << 8) | 18)	/* set iface remotw address */
#define SIOCGIFBRDADDR	(('S' << 8) | 19)	/* get iface ibroadcast address */
#define SIOCSIFBRDADDR	(('S' << 8) | 20)	/* set iface broadcast address */
#define SIOCGIFNETMASK	(('S' << 8) | 21)	/* get iface network mask */
#define SIOCSIFNETMASK	(('S' << 8) | 22)	/* set iface network mask */
#define SIOCGIFMETRIC	(('S' << 8) | 23)	/* get metric */
#define SIOCSIFMETRIC	(('S' << 8) | 24)	/* set metric */
#define SIOCGIFMTU	(('S' << 8) | 27)	/* get MTU size */
#define SIOCSIFMTU	(('S' << 8) | 28)	/* set MTU size */

/* routing table calls */
#define SIOCADDRT	(('S' << 8) | 30)	/* add routing table entry */
#define SIOCDELRT	(('S' << 8) | 31)	/* delete routing table entry */

/* ARP cache control calls */
#define SIOCDARP	(('S' << 8) | 40)	/* delete ARP table entry */
#define SIOCGARP	(('S' << 8) | 41)	/* get ARP table entry */
#define SIOCSARP	(('S' << 8) | 42)	/* set ARP table entry */

/* Structure used by Fxattr() */
typedef struct {
  unsigned short mode;		/* File type and access permissions */
  long index;			/* Inode */
  unsigned short dev;		/* Bios device */
  unsigned short reserved1;	/* Reserved */
  unsigned short nlink;		/* Number of links */
  unsigned short uid;		/* uid */
  unsigned short gid;		/* gid */
  long size;			/* File size in bytes */
  long blksize;			/* Block size */
  long nblocks;			/* Blocks used by file*/
  short mtime;			/* Time of last modification */
  short mdate;			/* Date of last modification */
  short atime;			/* Time of last access */
  short adate;			/* Date of last access */
  short ctime;			/* Time of file creation */
  short cdate;			/* Date of file creation */
  short attr;			/* Standard file attributes */
  short reserved2;		/* Reserved */
  long reserved3;		/* Reserved */
  long reserved4;		/* Reserved */
} _XATTR;

/* Structure used by Pmsg() */
typedef struct {
  long userlong1;		/* User message */
  long userlong2;		/* User message */
  short pid;			/* pid of reader or writer */
} _PMSG;

/* Structure used by Psigaction() */
typedef struct {
  long sa_handler;
  short sa_mask;
  short sa_flags;
} _SIGACTION;

/* Alternative structure names, according to Atari Compendium */
typedef _XATTR XATTR;
typedef _PMSG PMSG;
typedef _SIGACTION SIGACTION;


/* MiNT function prototypes */
typedef void voidfuncl(long);

long Dclosedir(long);
long Dcntl(short,char *,long);
long Dgetcwd(char *,short,short);
long Dlock(short,short);
long Dopendir(char *,short);
long Dpathconf(char *,short);
long Dreaddir(short,long,char *);
long Drewinddir(long);
long Fchmod(char *,short);
long Fchown(char *,short,short);
long Fcntl(short,long,short);
long Fgetchar(short,short);
long Finstat(short);
long Flink(char *,char *);
long Fmidipipe(short,short,short);
long Foutstat(short);
long Fpipe(short *);
long Fputchar(short,long,short);
long Freadlink(short,char *,char *);
short Fselect(short,long *,long *,long);
long Fsymlink(char *,char *);
long Fxattr(short,char *,XATTR *);
void Pause(void);
short Pdomain(short);
short Pfork(void);
short Pgetegid(void);
short Pgeteuid(void);
short Pgetgid(void);
short Pgetpgrp(void);
short Pgetpid(void);
short Pgetppid(void);
short Pgetuid(void);
short Pkill(short,short);
short Pmsg(short,long,PMSG *);
short Pnice(short);
long Prenice(short,short);
void Prusage(long *);
long Psemaphore(short,long,long);
short Psetgid(short);
long Psetlimit(short,long);
long Psetpgrp(short,short);
short Psetuid(short);
long Psigaction(short,SIGACTION *,SIGACTION *);
long Psigblock(long);
long Psignal(short,voidfuncl *);
long Psigpause(long);
long Psigpending(void);
void Psigreturn(void);
long Psigsetmask(long);
short Pumask(short);
long Pusrval(long);
short Pvfork(void);
long Pwait(void);
long Pwait3(short,long *);
long Pwaitpid(short,short,long *);
void Salert(char *);
void Syield(void);
long Sysconf(short);
long Talarm(long);


#endif /* _MINTBIND_H_ */
