/* EXEPAK self-extractor code stub

 (c)1997 Adam Ierymenko
 (c)2004-2005 Stefan Talpalaru

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 MA  02111-1307  USA */

/* Warning: this stub code is written to be as small and fast as possible,
 * not to be pretty.  Modifiers beware. */

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ucl/ucl.h>
#if defined(UCL_USE_ASM)
# include <ucl/ucl_asm.h>
# define ucl_nrv2e_decompress_8 ucl_nrv2e_decompress_asm_small_8
#endif
#include "stubsize.h"

void *memrchr(const void *s, int c, size_t n);
extern unsigned int mapcompressed[];

/* Define this to the file we should open to get the original program */
#define PROC_EXE "/proc/self/exe"

/* Marker */
static char marker[] = "EPK11";

/* Saves a bit of space rather than having this in every string */
static char *blah = "can't extract: ";

/* Everything here is done in main() to minimize code size */
/* We also use goto's in here to call redundant code to save space */
void main(int argc,char *argv[])
{
  int dfd = -1, writebufsize = 0;
  char ddname[15], dfname[272], *readbuf = (char *)mapcompressed[1], *writebuf, *frombuf;
  char *charlist = "abcdefg";
  unsigned int sizes[2]; /* [0] = comp block size, [1] = decomp block size */
  unsigned int destlen;
  char dontspawn = 0;
  int not_comp; //not compressed
  char *urand = "/dev/urandom";
  int ur, i, tries = 5;
  unsigned int rnd;
  
  /* for (i=0; i<200; i++) */
  /*   if (*(mapcompressed + i) == 33261) */
  /*     printf("%d\n", i); */
  
  /* printf("%d, %d.\n", readbuf[0], readbuf[1]); */
  
  /* Check the arguments for a first argument ordering us to decompress
   * to stdout */
  if ((argc > 1)&&(!strcmp(argv[1],">d")))
    dontspawn = 1;
  else
    {
      char *tmp;
      
      if ((ur = open(urand, O_RDONLY)) == -1)
	{
	  printf("%scan't open %s\n",blah,urand);
	  return;
	}
      maketmp:
      read(ur, &rnd, sizeof(unsigned int));
      sprintf(ddname, "/tmp/xpk");
      for (i=0; i<6; i++)
	{
	  sprintf(ddname + 8 + i, "%c", charlist[rnd % 7]);
	  rnd /= 7;
	}
      tmp = memrchr(argv[0], '/', strlen(argv[0]));
      sprintf(dfname, "%s/%s", ddname, tmp == 0 ? argv[0] : tmp + 1);
    }
  
  /* Open the temporary executable (unless we're outputting to stdout) */
  if (!dontspawn)
    {
      if ((mkdir(ddname, 0777)) < 0)
	{
	  tries--;
	  if (tries)
	    goto maketmp;
	  printf("%scan't open %s\n",blah,ddname);
	  return;
	}
      if ((dfd = open(dfname,O_WRONLY|O_CREAT|O_EXCL,mapcompressed[0])) <= 0)
	{
	  rmdir(ddname);
	  tries--;
	  if (tries)
	    goto maketmp;
	  printf("%scan't open %s\n",blah,dfname);
	  return;
	}
    }

  /* Assumption:
   * - the blocks have the same uncompressed size (or smaller - the last one)*/
  if (!(writebuf = malloc((writebufsize = readbuf[1]))))
    {
      close(dfd);
      unlink(dfname);
      rmdir(ddname);
      printf("%sout of memory\n",blah);
      return;
    }

  while (1)
    {
      /* The compressed data in the compressed executable is blocked, with
       * each compressed block being preceded with two 32-bit integers in
       * whatever byte order this host uses telling the size of the compressed
       * block and the size of the decompressed block. */
      if ((sizes[0] = readbuf[0]) == 0)
	break;
      sizes[1] = readbuf[1];
      readbuf += sizeof(sizes);
  
      
      /* Read the not/compressed data block */
      if (sizes[0] != sizes[1]) //compressed
	{
	  not_comp = 0;
	  /* Decompress */
	  if (ucl_nrv2e_decompress_8(readbuf,sizes[0],writebuf,&destlen, NULL) != UCL_E_OK)
	    {
	      executable_corrupted:
	      printf("%scorrupt\n",blah);
	      close(dfd);
	      unlink(dfname);
	      return;
	    }
	  
	  /* Sanity check */
	  if (destlen != sizes[1])
	    goto executable_corrupted;
	  
	  frombuf = writebuf;
	}
      else //not compressed
	{
	  not_comp = 1;
	  frombuf = readbuf;
	}
      
      /* Write decompressed data */
      if (dontspawn)
	write(STDOUT_FILENO,frombuf,sizes[1]);
      else
	{
	  if (write(dfd,frombuf,sizes[1]) != sizes[1])
	    {
	      printf("%swrite error\n",blah);
	      close(dfd);
	      unlink(dfname);
	      rmdir(ddname);
	      return;
	    }
	}
      readbuf += sizes[0];
    }

  close(dfd);

	/* If we've been ordered to permanently decompress, don't spawn */
  if (dontspawn)
    return;

	/* Fork off a subprocess to clean up */
	/* We have to do this stupid double-fork trick to keep a zombie from
	 * hanging around if the spawned original program doesn't check for
	 * subprocesses.  (As well as to prevent the real program from getting
	 * confused about this subprocess it shouldn't have) */
  if (!fork())
    {
      if (!fork())
	{
	  sleep(1);
	  unlink(dfname);
	  rmdir(ddname);
	  return;
	}
      return;
    }

	/* Wait for the first fork()'d process to die */
  wait((int *)0);

	/* Spawn the original process */
  execv(dfname,argv);
  
  __asm__ __volatile__ ("\
.bss\n\
.globl mapcompressed\n\
mapcompressed:\n\
.text\n");
}
