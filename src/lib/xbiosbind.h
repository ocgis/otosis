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

#endif /* _XBIOSBIND_H_ */
