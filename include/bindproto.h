/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1998 Tomas Berndtsson <tomas@nocrew.org>
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

#ifndef BINDPROTO_H
#define BINDPROTO_H

#include <stdio.h>
#include "config.h"

#define so(t) sizeof(t)

/* pushing a value safely into the stack */
#ifdef USE_DIRECT_MOVES
#define push(offset,value,type) *((type *)(&stack[offset])) = value;
#else
#define push(offset,value,type) \
  if (sizeof(type) == 4) { \
    stack[offset]   = ((UInt32)value >> 24) & 0xff; \
    stack[offset+1] = ((UInt32)value >> 16) & 0xff; \
    stack[offset+2] = ((UInt32)value >> 8)  & 0xff; \
    stack[offset+3] = ((UInt32)value)       & 0xff; \
  } else if (sizeof(type) == 2) { \
    stack[offset]   = ((UInt32)value >> 8)  & 0xff; \
    stack[offset+1] = ((UInt32)value)       & 0xff; \
  } else { \
    fprintf (stderr, "otosis: bindproto.h: push not defined for size %d\n", sizeof (type)); \
  }
#endif

#define bindproto0(group,ret,name,no) \
ret name(void) \
{ \
  static char stack[2]; \
  push(0,no,short); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto1(group,ret,name,no,t1) \
ret name(t1 a1) \
{ \
  static char stack[2+so(t1)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto2(group,ret,name,no,t1,t2) \
ret name(t1 a1, t2 a2) \
{ \
  static char stack[2+so(t1)+so(t2)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto3(group,ret,name,no,t1,t2,t3) \
ret name(t1 a1, t2 a2, t3 a3) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto4(group,ret,name,no,t1,t2,t3,t4) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto5(group,ret,name,no,t1,t2,t3,t4,t5) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4),a5,t5); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto6(group,ret,name,no,t1,t2,t3,t4,t5,t6) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4),a5,t5); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5),a6,t6); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto7(group,ret,name,no,t1,t2,t3,t4,t5,t6,t7) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6, t7 a7) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t7)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4),a5,t5); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5),a6,t6); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6),a7,t7); \
  return (ret)dispatch_##group(stack); \
}

#define bindproto9(group,ret,name,no,t1,t2,t3,t4,t5,t6,t7,t8,t9) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6, t7 a7, t8 a8, t9 a9) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t7)+so(t8)+so(t9)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4),a5,t5); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5),a6,t6); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6),a7,t7); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t8),a8,t8); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t8)+so(t9),a9,t9); \
  return (ret)dispatch_##group(stack); \
}



/* Prototypes without return value needs their own defines */

#define bindproto0v(group,name,no) \
void name(void) \
{ \
  static char stack[2]; \
  push(0,no,short); \
  (void)dispatch_##group(stack); \
}

#define bindproto1v(group,name,no,t1) \
void name(t1 a1) \
{ \
  static char stack[2+so(t1)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  (void)dispatch_##group(stack); \
}

#define bindproto2v(group,name,no,t1,t2) \
void name(t1 a1, t2 a2) \
{ \
  static char stack[2+so(t1)+so(t2)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  (void)dispatch_##group(stack); \
}

#define bindproto3v(group,name,no,t1,t2,t3) \
void name(t1 a1, t2 a2, t3 a3) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  (void)dispatch_##group(stack); \
}

#define bindproto4v(group,name,no,t1,t2,t3,t4) \
void name(t1 a1, t2 a2, t3 a3, t4 a4) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  (void)dispatch_##group(stack); \
}

#define bindproto5v(group,name,no,t1,t2,t3,t4,t5) \
void name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4),a5,t5); \
  (void)dispatch_##group(stack); \
}

#define bindproto6v(group,name,no,t1,t2,t3,t4,t5,t6) \
void name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)]; \
  push(0,no,short); \
  push(2,a1,t1); \
  push(2+so(t1),a2,t2); \
  push(2+so(t1)+so(t2),a3,t3); \
  push(2+so(t1)+so(t2)+so(t3),a4,t4); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4),a5,t5); \
  push(2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5),a6,t6); \
  (void)dispatch_##group(stack); \
}




#endif /* BINDPROTO_H */
