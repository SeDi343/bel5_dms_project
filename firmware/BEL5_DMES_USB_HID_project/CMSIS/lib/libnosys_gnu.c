/**********************************************************************
* $Id$		libnosys_gnu.c  				2008-12-01
*//**
* @file		libnosys_gnu.c
* @brief	Definitions for OS interface, stub function required by newlibc
* 			used by Codesourcery GNU compiler.
* @version
* @date		01. December. 2008
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#if defined(__GNUC__)

#include "lpc_types.h"
#include <errno.h>
#include <sys/times.h>
#include <sys/stat.h>

/* errno definition */
#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

int _close(int file)
{
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}
int _exit()
{
  return 0;
}

int _fork()
{
  errno = EAGAIN;
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _getpid()
{
  return 1;
}

int _kill(int pid, int sig)
{
  errno = EINVAL;
  return(-1);
}

int isatty(int fildes)
{
  return 1;
}
int _isatty(int fildes)
{
  return 1;
}

int _link(char *old, char *new)
{
  errno = EMLINK;
  return -1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _open(const char *name, int flags, int mode)
{
  return -1;
}

int _read(int file, char *ptr, int len)
{
  return 0;
}

caddr_t _sbrk(int incr)
{
  extern char __cs3_heap_end;		/* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0)
  {
    heap_end = &__cs3_heap_end;
    /* give 16KB area for stacks and use the rest of memory for heap*/
    heap_end += 0x1000; //0x4000
  }
  prev_heap_end = heap_end;

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _stat(char *file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}
int _times(struct tms *buf)
{
  return -1;
}

int _unlink(char *name)
{
  errno = ENOENT;
  return -1;
}

int _wait(int *status)
{
  errno = ECHILD;
  return -1;
}

int _write(int file, char *ptr, int len)
{
  return 0;
}

#endif /*__GNUC__*/
