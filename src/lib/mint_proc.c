/************************************************************************
 *
 *  oTOSis - TOS emulator for Linux/68K
 *
 *  Copyright 1996 Elias Martenson <elias@omicron.se>
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

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include "div.h"
#include "prototypes.h"
#include "toserrors.h"
#include "mint.h"

#ifdef _SEM_SEMUN_UNDEFINED
union semun {
  int                  val;
  struct semid_ds *    buf;
  unsigned short int * array;
  struct seminfo *     __buf;
};
#endif

extern TosProgram *prog;

MINTFUNC(Syield)
{
  /*
   *  This function does nothing
   */
  return TOS_E_OK;
}

MINT_UNIMP(Fpipe);
MINT_UNIMP(Fgetchar);
MINT_UNIMP(Fputchar);
MINT_UNIMP(Pwait);

MINTFUNC(Pnice)
{
  /*
   *  Changing of process priorities is not supported
   *  via this emulator
   */
  return 0;
}

MINTFUNC(Pgetpid)
{
  return getpid();
}

MINTFUNC(Pgetppid)
{
  return getppid();
}

static int pgrp = 0;
MINTFUNC(Pgetpgrp)
{
  /*
   *  This isn't useful until we have a way of emulating signals
   */
  return pgrp;
}

MINTFUNC(Psetpgrp)
{
  TOSARG(short,pid);
  TOSARG(short,newgrp);

  pgrp = newgrp;
  return pgrp;
}

MINTFUNC(Pgetuid)
{
  return getuid();
}

MINTFUNC(Psetuid)
{
  TOSARG(short,uid);

  if( setuid( uid ) == -1 ) {
    return translate_error( errno );
  }
  return uid;
}

MINTFUNC(Pkill)
{
  TOSARG(short,pid);
  TOSARG(short,sig);

  if( kill( pid, sig ) == -1 ) {
    return translate_error( errno );
  }
  return 0;
}

MINT_UNIMP(Psignal);

MINTFUNC(Pvfork)
{
  return mint_Pfork( _args );
}

MINTFUNC(Pgetgid)
{
  return getgid();
}

MINTFUNC(Psetgid)
{
  TOSARG(short,gid);

  if( setgid( gid ) == -1 ) {
    return translate_error( errno );
  }
  return gid;
}

/*
 *  Same thing applies here as to the Psetpgrp and Pgetpgrp
 *  calls. Signal emulation needs to be implemented first.
 */
MINTFUNC(Psigblock)
{
  return 0;
}

MINT_UNIMP(Psigsetmask);

MINTFUNC(Pusrval)
{
  TOSARG(long,val);
  long oldval = prog->usrval;

  if( val != -1 ) {
    prog->usrval = val;
  }
  return oldval;
}

MINTFUNC(Pdomain)
{
  TOSARG(short,newdomain);
  int olddomain = prog->domain;

  if( newdomain == 0 || newdomain == 1 ) {
    prog->domain = newdomain;
  }

  return olddomain;
}

MINT_UNIMP(Psigreturn);

MINTFUNC(Pfork)
{
  int pid;

  if( (pid = fork()) == -1 ) {
    return translate_error( errno );
  }
  return pid;
}

MINT_UNIMP(Pwait3);
MINT_UNIMP(Fselect);
MINT_UNIMP(Prusage);
MINT_UNIMP(Psetlimit);
MINT_UNIMP(Talarm);
MINT_UNIMP(Pause);

MINTFUNC(Sysconf)
{
  TOSARG(short,val);

  /*
   *  The return values of these are not really correct
   */
  switch( val ) {
  case -1:			/* Maximum value for val */
    return 4;
  case 0:			/* Max memory regions per process */
    return -1;
  case 1:			/* Max length of command string */
    return -1;
  case 2:			/* Max open files per process */
    return -1;
  case 3:			/* Max supplementary group ID's */
    return -1;
  case 4:			/* Max processes per user */
    return -1;
  }
  return TOS_ERROR;
}

MINT_UNIMP(Psigpending);


/* Defines used in Pmsg */
#define FIX_BOXID(id) ((id) & 0x7ffffff)
#define OUR_BOX 0x12345678

/* Fix error messages!! */

/*
** Description
** Send or receive message to or from message box
**
** 1998-09-05 CG
*/
static long internal_Pmsg(short mode, long mboxid, PMSG * msg)
{
  int  msqid;
  struct {
    int  type;
    PMSG msg;
  } ipc_msg;


  if (mode == TOS_PMSG_WRITE) {
    int err;

    msqid = msgget (OUR_BOX, 0666 | IPC_CREAT);
    
    if (msqid < 0) {
      return -1;
    }

    ipc_msg.type = FIX_BOXID (mboxid);
    ipc_msg.msg = *(PMSG *)msg;
    ipc_msg.msg.pid = getpid ();

    err = msgsnd (msqid, &ipc_msg, sizeof (ipc_msg.msg), 0);

    if (err < 0) {
      fprintf (stderr, "Pmag: TOS_PMSG_WRITE: Send data: error %d\n", err);
      
      return -1;
    }

    /* Wait for acknowledge */
    err = msgrcv (msqid,
                  &ipc_msg, sizeof (ipc_msg.msg),
                  FIX_BOXID (0xffff0000 | ipc_msg.msg.pid),
                  0);
    
    if (err < 0) {
      fprintf (stderr, "Pmsg: TOS_PMSG_WRITE: Wait for ack: error %d, box 0x%x\n",
               err,
               FIX_BOXID (0xffff0000 |ipc_msg.msg.pid));
      perror ("Pmsg");
      return -1;
    }
    ((PMSG *)msg)->pid = ipc_msg.msg.pid;
  } else if (mode == TOS_PMSG_READ) {
    int err;

    msqid = msgget (OUR_BOX, 0666 | IPC_CREAT);
    
    if (msqid < 0) {
      return -1;
    }
    
    err = msgrcv (msqid,
                  &ipc_msg,
                  sizeof (ipc_msg.msg),
                  FIX_BOXID (mboxid),
                  0);

    if (err < 0) {
      fprintf (stderr, "Pmsg: TOS_PMSG_READ: Receive: error %d\n", err);
      
      return -1;
    }

    *(PMSG *)msg = ipc_msg.msg;

    /* Send acknowledge */
    ipc_msg.type = FIX_BOXID (0xffff0000 | ipc_msg.msg.pid);
    ipc_msg.msg.pid = getpid ();
    
    err = msgsnd (msqid, &ipc_msg, sizeof (ipc_msg.msg), 0);

    if (err < 0) {
      fprintf (stderr, "Pmsg: TOS_PMSG_READ: Send ack: error %d\n", err);

      return -1;
    }
  } else if (mode == TOS_PMSG_WRITEREAD ) {
    if (internal_Pmsg (TOS_PMSG_WRITE, mboxid, msg) == -1) {
      return -1;
    } else {
      return internal_Pmsg (TOS_PMSG_READ, 0xffff0000 | getpid (), msg);
    }
  } else {
    fprintf (stderr, "Pmsg: Unhandled mode 0x%x", mode);
    return -1;
  }

  return 0;
}

MINTFUNC(Pmsg)
{
  TOSARG(short,mode);
  TOSARG(long,mboxid);
  TOSARG(PMSG *,msg);
  
  return internal_Pmsg(mode, mboxid, msg);
}

MINT_UNIMP(Fmidipipe);

MINTFUNC(Prenice)
{
  /*
   *  Process priorites are not emulated
   */
  return 0;
}

MINT_UNIMP(Flink);
MINT_UNIMP(Fsymlink);
MINT_UNIMP(Dcntl);

MINTFUNC(Pumask)
{
  TOSARG(short,mode);

  return umask( mode );
}


/* Fix error messages!! */

/*
** Description
** Create a semaphore which may only be accessed by one process at a time.
**
** 1998-09-05 CG
*/
MINTFUNC(Psemaphore)
{
  TOSARG(short,mode);
  TOSARG(long,id);
  TOSARG(long,timeout);

  switch (mode) {
  case TOS_SEM_CREATE:
  {
    int         semid;
    union semun arg;
    
    semid = semget (id, 1, 0666 | IPC_CREAT /*| IPC_EXCL*/);
    
    /* Was there an error? */
    if (semid < 0) {
      fprintf (stderr, "otosis: Psemaphore: TOS_SEM_CREATE: semid = %d\n", semid);
      perror ("otosis");
      return -1;
    }
    
    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
      fprintf (stderr, "otosis: Psemaphore: TOS_SEM_CREATE: Couldn't set initial value\n");
      perror("otosis");
      return -1;
    }

    return 0;
  }

  case TOS_SEM_LOCK:
  {
    int           semid;
    struct sembuf sb = {0, -1, 0};

    semid = semget (id, 1, 0);

    if (semop (semid, &sb, 1) < 0) {
      fprintf (stderr, "otosis: Psemaphore: TOS_SEM_LOCK: couldn't lock\n");
      perror ("otosis");
      return -1;
    }

    return 0;
  }
  case TOS_SEM_UNLOCK:
  {
    int           semid;
    struct sembuf sb = {0, 1, 0};

    semid = semget (id, 1, 0);

    if (semop (semid, &sb, 1) < 0) {
      fprintf (stderr, "otosis: Psemaphore: TOS_SEM_LOCK: couldn't lock\n");
      perror ("otosis");
      return -1;
    }

    return 0;
  }
  case TOS_SEM_DESTROY:
  {
    int         semid;
    union semun arg;

    semid = semget (id, 1, 0);

    if (semid < 0) {
      fprintf (stderr, "otosis: Psemaphore: TOS_SEM_DESTROY: Couldn't get semaphore\n");
      perror("otosis");
      return -1;
    }

    if (semctl(semid, 0, IPC_RMID, arg) < 0) {
      fprintf (stderr, "otosis: Psemaphore: TOS_SEM_DESTROY: Couldn't remove semaphore\n");
      perror("otosis");
      return -1;      
    }

    return 0;
  }
  default :
    fprintf (stderr, "otosis: Psemaphore: Unknown mode %d\n", mode);
    return -1;
  }
}

MINT_UNIMP(Dlock);
MINT_UNIMP(Psigpause);
MINT_UNIMP(Psigaction);

MINTFUNC(Pgeteuid)
{
  return geteuid();
}

MINTFUNC(Pgetegid)
{
  return getegid();
}

MINT_UNIMP(Pwaitpid);
MINT_UNIMP(Salert);

/* Local Variables:              */
/* tab-width: 8                  */
/* compile-command: "make -C .." */
/* End:                          */
