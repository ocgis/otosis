/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
 *  Copyright 1996 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ************************************************************************/

#ifndef TOSERRORS_H
#define TOSERRORS_H

#define TOS_E_OK	0	/* No error */
#define TOS_ERROR	-1	/* Generic error */
#define TOS_EDRVNR	-2	/* Drive not ready */
#define TOS_EUNCMD	-3	/* Unknown command */
#define TOS_E_CRC	-4	/* CRC error */
#define TOS_EBADRQ	-5	/* Bad request */
#define TOS_E_SEEK	-6	/* Seek error */
#define TOS_EMEDIA	-7	/* Unknown media */
#define TOS_ESECNF	-8	/* Sector not found */
#define TOS_EPAPER	-9	/* Out of paper */
#define TOS_EWRITF	-10	/* Write fault */
#define TOS_EREADF	-11	/* Read fault */
#define TOS_EWRPRO	-12	/* Write protected */
#define TOS_E_CHNG	-14	/* Media change */
#define TOS_EUNDEV	-15	/* Unknown device */
#define TOS_EBADSF	-16	/* Bad sectors on format */
#define TOS_EOTHER	-17	/* Insert other disk */

#define TOS_EINVFN	-32	/* Invalid function */
#define TOS_EFILNF	-33	/* File not found */
#define TOS_EPTHNF	-34	/* Path not found */
#define TOS_ENHNDL	-35	/* No more handles */
#define TOS_EACCDN	-36	/* Access denied */
#define TOS_EIHNDL	-37	/* Invalid handle */
#define TOS_EINSMEM	-39	/* Insufficient memory */
#define TOS_EIMBA	-40	/* Invalid memory block address */
#define TOS_EDRIVE	-46	/* Invalid drive specification */
#define TOS_ENSAME	-48	/* Cross-device rename */
#define TOS_ENMFIL	-49	/* No more files */
#define TOS_ELOCKED	-58	/* Record is already locked */
#define TOS_ENSLOCK	-59	/* Invalid lock removal request */
#define TOS_ERANGE	-64	/* Range error */
#define TOS_ENAMETOOLONG TOS_ERANGE
#define TOS_EINTRN	-65	/* Internal error */
#define TOS_EPLFMT	-66	/* Invalid program load format */
#define TOS_EGSBF	-67	/* Memory block growth failure */
#define TOS_ELOOP	-80	/* Too many symbolic links */
#define TOS_EMOUNT	-200	/* Mount point crossed */

#define	LAST_TOS_ERRNO	-200

#endif

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
