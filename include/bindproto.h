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

#define so(t) sizeof(t)

#define bindproto0(group,ret,name,no) \
ret name(void) \
{ \
  static char stack[2]; \
  *((short *)(stack)) = no; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto1(group,ret,name,no,t1) \
ret name(t1 a1) \
{ \
  static char stack[2+so(t1)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto2(group,ret,name,no,t1,t2) \
ret name(t1 a1, t2 a2) \
{ \
  static char stack[2+so(t1)+so(t2)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto3(group,ret,name,no,t1,t2,t3) \
ret name(t1 a1, t2 a2, t3 a3) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto4(group,ret,name,no,t1,t2,t3,t4) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto5(group,ret,name,no,t1,t2,t3,t4,t5) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  *(   (t5 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4))) = a5; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto6(group,ret,name,no,t1,t2,t3,t4,t5,t6) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  *(   (t5 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4))) = a5; \
  *(   (t6 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5))) = a6; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto7(group,ret,name,no,t1,t2,t3,t4,t5,t6,t7) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6, t7 a7) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t7)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  *(   (t5 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4))) = a5; \
  *(   (t6 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5))) = a6; \
  *(   (t7 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6))) = a7; \
  return (ret)dispatch_##group(stack); \
}

#define bindproto9(group,ret,name,no,t1,t2,t3,t4,t5,t6,t7,t8,t9) \
ret name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6, t7 a7, t8 a8, t9 a9) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t7)+so(t8)+so(t9)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  *(   (t5 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4))) = a5; \
  *(   (t6 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5))) = a6; \
  *(   (t7 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6))) = a7; \
  *(   (t8 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t7))) = a8; \
  *(   (t9 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)+so(t7)+so(t8))) = a9; \
  return (ret)dispatch_##group(stack); \
}



/* Prototypes without return value needs their own defines */

#define bindproto0v(group,name,no) \
void name(void) \
{ \
  static char stack[2]; \
  *((short *)(stack)) = no; \
  (void)dispatch_##group(stack); \
}

#define bindproto1v(group,name,no,t1) \
void name(t1 a1) \
{ \
  static char stack[2+so(t1)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  (void)dispatch_##group(stack); \
}

#define bindproto2v(group,name,no,t1,t2) \
void name(t1 a1, t2 a2) \
{ \
  static char stack[2+so(t1)+so(t2)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  (void)dispatch_##group(stack); \
}

#define bindproto3v(group,name,no,t1,t2,t3) \
void name(t1 a1, t2 a2, t3 a3) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  (void)dispatch_##group(stack); \
}

#define bindproto4v(group,name,no,t1,t2,t3,t4) \
void name(t1 a1, t2 a2, t3 a3, t4 a4) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  (void)dispatch_##group(stack); \
}

#define bindproto5v(group,name,no,t1,t2,t3,t4,t5) \
void name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  *(   (t5 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4))) = a5; \
  (void)dispatch_##group(stack); \
}

#define bindproto6v(group,name,no,t1,t2,t3,t4,t5,t6) \
void name(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) \
{ \
  static char stack[2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5)+so(t6)]; \
  *((short *)(stack)) = no; \
  *(   (t1 *)(stack+2)) = a1; \
  *(   (t2 *)(stack+2+so(t1))) = a2; \
  *(   (t3 *)(stack+2+so(t1)+so(t2))) = a3; \
  *(   (t4 *)(stack+2+so(t1)+so(t2)+so(t3))) = a4; \
  *(   (t5 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4))) = a5; \
  *(   (t6 *)(stack+2+so(t1)+so(t2)+so(t3)+so(t4)+so(t5))) = a6; \
  (void)dispatch_##group(stack); \
}




#endif /* BINDPROTO_H */
