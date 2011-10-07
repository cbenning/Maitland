/* EXEPAK version 1.2

 (c)1997 Adam Ierymenko
 Copyright (C) 2004 - 2005 by Stefan Talpalaru <stefantalpalaru@yahoo.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 MA  02111-1307  USA

 This code is not pretty, but it works. :) */

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <utime.h>
#include <time.h>
#include "../stub/__stub.h"
#include "../stub/stubsize.h"
#include <ucl/ucl.h>
#include <errno.h>
#include <linux/elf.h>
#include <sys/mman.h>

/* Prototypes */
char *tospaces(char *str);
char my_rename(char *oldname,char *newname);
char check_already_compressed(char *fname);
char create_sfx_binary(char *infile);
char decompress_sfx_binary(char *infile);
void bad_options(void);

/* Configuration options */
ucl_uint blocksize = (256*1024); // 256 Kb - seems reasonable
char testing = 0;
char extracting = 0;
char file_buffer[65536];
int stub_wsize;
int zbufsz = 0;

/* Cheezy little formatting function */
char *tospaces(char *str)
{
  static char buf[512];
  register int i = 0;

  while(*(str+i))
    buf[i++] = ' ';
  buf[i] = '\0';

  return buf;
}

/*
 * Rename function which will work between two different filesystems.
 * Returns nonzero if error, zero if successful.
 *
 * (I have /home and /tmp mounted on two different disks)
 */
char my_rename(char *oldname,char *newname)
{
  FILE *infile,*outfile;
  char buf[65536];
  register int n;

  errno = 0;
  if (rename(oldname,newname))
    {
      if (errno == EXDEV)
	{
	  if (!(outfile = fopen(newname,"w")))
	    return 1;
	  if (!(infile = fopen(oldname,"r")))
	    {
	      fclose(outfile);
	      return 1;
	    }
	  while((n = fread(&buf,1,sizeof(buf),infile)) > 0)
	    {
	      if (fwrite(&buf,1,n,outfile) != n)
		{
		  fclose(outfile);
		  return 1;
		}
	    }
	  fclose(infile);
	  fclose(outfile);
	  unlink(oldname);
	}
      else return 1;
    }
  return 0;
}

/*
 * read_headers() reads the ELF header and the program header table,
 * and checks to make sure that this is in fact a file that we should
 * be munging.
 */

int read_headers(int fd)
{
  Elf32_Ehdr elfhdr;
  Elf32_Ehdr *elfhdr2 = (Elf32_Ehdr *)exepak_stub;
  Elf32_Phdr *phdr = (Elf32_Phdr *)&exepak_stub[elfhdr2->e_phoff];

  /* written stub size */
  stub_wsize = phdr[1].p_offset + phdr[1].p_filesz;

  if( read(fd, &elfhdr, sizeof(elfhdr)) != sizeof(elfhdr) )
    return 0;
  if( elfhdr.e_ident[EI_MAG0] != ELFMAG0
      || elfhdr.e_ident[EI_MAG1] != ELFMAG1
      || elfhdr.e_ident[EI_MAG2] != ELFMAG2
      || elfhdr.e_ident[EI_MAG3] != ELFMAG3
      || elfhdr.e_type != ET_EXEC )
    return 0;
  if( elfhdr.e_ehsize != sizeof(Elf32_Ehdr) )
    return 0;

  lseek(fd,0,SEEK_SET);
  return 1;
}

int is_script(int fd)
{
  char magic[2];
  
  lseek(fd, 0, SEEK_SET);
  if(read(fd, &magic, 2) != 2)
    return 0;
  if(memcmp(magic, "#!", 2)) // 0 if equal
    return 0;
    
  lseek(fd, 0, SEEK_SET);
  return 1;
}

int append_stubfile(int fdcompressed,int fdout,char *stub,struct stat *infstat,unsigned int compressed_size)
{
  Elf32_Ehdr *elfhdr = (Elf32_Ehdr *)stub;
  Elf32_Phdr *phdr = (Elf32_Phdr *)&stub[elfhdr->e_phoff];
  unsigned int mode;
  void  *map = NULL;

    /* permission for decompression */
  mode = infstat->st_mode;
    /* paranoia check */
  if( elfhdr->e_ident[EI_MAG0] != ELFMAG0
      || elfhdr->e_ident[EI_MAG1] != ELFMAG1
      || elfhdr->e_ident[EI_MAG2] != ELFMAG2
      || elfhdr->e_ident[EI_MAG3] != ELFMAG3
      || elfhdr->e_type != ET_EXEC )
    return 0;
  if( elfhdr->e_ehsize != sizeof(Elf32_Ehdr) )
    return 0;
  if( !elfhdr->e_phoff && elfhdr->e_phnum != 2 &&
      elfhdr->e_phentsize != sizeof(Elf32_Phdr) )
    return 0;
  
  /*
   * Remove references to the section header table if
   * it was removed, and reduces program header table entries that
   * included truncated bytes at the end of the file.
   */
  elfhdr->e_shoff = 0;
  elfhdr->e_shnum = 0;
  elfhdr->e_shentsize = 0;
  elfhdr->e_shstrndx = 0;
  
  zbufsz = phdr[1].p_memsz - phdr[1].p_filesz - 8;
//  printf("zbufsz = 0x%X\n", zbufsz);
  
  /*
   * Adjust the size of the data section. Add
   * an integer at the end to break decompress loop.
   */
  phdr[1].p_filesz += 8 + compressed_size;
  phdr[1].p_memsz  += 8 + compressed_size + sizeof(int);

  /*
   * Write the new ordered stub file at the begin
   * to the compressed file and sets the new file size.
   */

//  printf("the stub is %d bytes long\n", stub_wsize + 8);
  if(write(fdout,stub,stub_wsize) != stub_wsize)
    return 0;
  if(write(fdout,&compressed_size,sizeof(int)) != sizeof(int))
    return 0;
  if(write(fdout,&mode,sizeof(mode)) != sizeof(mode))
    return 0;

  if((map = mmap(0, compressed_size, PROT_READ, MAP_SHARED, fdcompressed, 0)) == (void *)-1)
    return 0;
  
  if(write(fdout, map + compressed_size - zbufsz, zbufsz) !=
     zbufsz) //the tail
    return 0;
  if(write(fdout, map, compressed_size - zbufsz) !=
     (compressed_size - zbufsz)) //the head
    return 0;

  munmap(map, compressed_size);
  
  return 1;
}

/*
 * This function checks to see if a file is already compressed by looking
 * for the marker string within the first 16k of the file (or less if the
 * file is smaller). Returns nonzero if true.
 */
char check_already_compressed(char *fname)
{
  FILE *inf;
  char buf[16400];
  int n;
  register int i;
  struct stat infstat;

  if (stat(fname,&infstat))
    return 0;

  /* If it's not an executable, return 0 */
  if ((!(infstat.st_mode & S_IXUSR))&&(!(infstat.st_mode & S_IXGRP)))
    {
      if (!(infstat.st_mode & S_IXOTH))
	return 0;
    }
  if (S_ISDIR(infstat.st_mode))
    return 0;

  if (!(inf = fopen(fname,"r")))
    return 0;
  n = fread(&buf,1,16384,inf);
  if (n > 0)
    {
      char exepak[] = {68, 87, 68, 79, 64, 74, 0};
      char epk11[] = {68, 79, 74, 48, 48, 0};

      /* make it say "EXEPAK" */
      for (i = 0; i < 6; i++)
	exepak[i]++;

      /* and this one: "EPK11" */
      for (i = 0; i < 5; i++)
	epk11[i]++;

      /* Get rid of nulls so we can use strstr() */
      for(i=0;i<n;i++)
	{
	  if (buf[i] == '\0')
	    buf[i] = (char)1;
	}
      buf[n+1] = '\0';
      if (strstr(buf,exepak))
	return 1;
      if (strstr(buf,epk11))
	return 2;
    }
  fclose(inf);
  return 0;
}

/*
 * Compresses an uncompressed executable.
 * Returns nonzero if successful.
 */
char create_sfx_binary(char *infile)
{
  unsigned int sizes[2]; /* Two 32-bit ints to write before each block */
  ucl_bytep inbuf;
  ucl_bytep outbuf;
  ucl_bytep frombuf;
  ucl_uint outsize;
  int n;
  FILE *inf,*of;
  struct stat infstat;
  char ofname[256];
  char newfname[256];
  int oldsize,newsize;
  int progmeter = 0;
  struct utimbuf utb;
  char didsomething = 0;
  int uncomp; //uncompressible
  int fdin = -1;

  sprintf(ofname,"/tmp/_comp%x%x",(unsigned int)time(NULL),getpid());
  sprintf(newfname,"%s~",infile);

  /* Allocate memory */
  if ((!(inbuf = (ucl_bytep)malloc(blocksize))) ||
      (!(outbuf = (ucl_bytep)malloc(blocksize / 8 + blocksize + 256))))
    {
      printf("%s: compress failed: out of memory!\n",infile);
      return 0;
    }

  /* Stat the input file */
  if (stat(infile,&infstat))
    {
      printf("%s: compress failed: can't stat\n",infile);
      return 0;
    }

  /* Can't compress files we don't own unless we're super-user */
  if ((getuid())&&(infstat.st_uid != getuid()))
    {
      printf("%s: compress failed: permission denied\n",infile);
      return 0;
    }

  /* Check to make sure it's an executable file */
  if ((!(infstat.st_mode & S_IXUSR))&&(!(infstat.st_mode & S_IXGRP)))
    {
      if (!(infstat.st_mode & S_IXOTH))
	{
	  printf("%s: compress failed: not an executable file\n",infile);
	  return 0;
	}
    }
  if (S_ISDIR(infstat.st_mode))
    {
      printf("%s: compress failed: %s is a directory\n",infile,infile);
      return 0;
    }

  /* Open the input file */
  if (!(inf = fopen(infile,"r")))
    {
      printf("%s: compress failed: %s\n",infile,strerror(errno));
      return 0;
    }

  /* Check to make sure if it's ELF it's an executable (not a dll) */
  if(! (read_headers(fileno(inf)) || is_script(fileno(inf))))
    {
      printf("%s: compress failed, invalid ELF binary or script.\n",infile);
      return 0;
    }

  /* Open the output file */
  if (!(of = fopen(ofname,"w+")))
    {
      printf("%s: compress failed: could not create temporary file\n",infile);
      return 0;
    }
  
  printf("Compressing...");
  fflush(stdout);
  
  /* Copy and compress infile to outfile */
  while((n = fread(inbuf,1,blocksize,inf)))
    {
      didsomething = 1;
      uncomp = 0;

      /* Compress the block of read data */
      if (ucl_nrv2e_99_compress(inbuf, n, outbuf, &outsize, 0, 10, NULL, NULL) != UCL_E_OK)
	{
	  printf("\r%s: compress failed: internal compression error\n",infile);
	  fclose(of);
	  unlink(ofname);
	  return 0;
	}
      /* Write the block size before the actual block */
      if (outsize >= n) //uncompressible block, write it as it is
	{
	  uncomp = 1;
	  outsize = n;
	}

      sizes[0] = outsize;
      sizes[1] = n;
      if (fwrite(&sizes,1,sizeof(sizes),of) != sizeof(sizes))
	{
	  printf("\r%s: compress failed: error writing to temporary file: %s\n",infile,strerror(errno));
	  fclose(of);
	  unlink(ofname);
	  return 0;
	}
      /* Write the block of un/compressed data */
      if (uncomp)
	frombuf = inbuf;
      else
	frombuf = outbuf;

      if (fwrite(frombuf,1,outsize,of) != outsize)
	{
	  printf("\r%s: compress failed: error writing to temporary file: %s\n",infile,strerror(errno));
	  fclose(of);
	  unlink(ofname);
	  return 0;
	}
      printf("\r%s: read=%d/%d written=%d (%.02f:1)",infile,(progmeter += n),(int)infstat.st_size,(int)ftell(of),(((float)n) / ((float)outsize)));
      fflush(stdout);
    }
  printf("\n");

  if (!didsomething)
    {
      printf("\r%s: compress failed: read error\n",infile);
      fclose(of);
      unlink(ofname);
      return 0;
    }

  oldsize = ftell(inf);
  newsize = ftell(of);

  /* Check to see if we got something smaller than before, if not we fail */
  if (newsize + stub_wsize >= oldsize)
    {
      printf("\r%s: compress failed: no reduction in size was achieved\n",infile);
      my_rename(newfname,infile);
      fclose(of);
      unlink(ofname);
      return 0;
    }
  fclose(inf);
  /* Reopen and write stub file to disk */
  if ((fdin = open(infile,O_WRONLY|O_CREAT|O_TRUNC)) < 0 || !append_stubfile(fileno(of), fdin, exepak_stub, &infstat, newsize))
    {
      printf("\r%s: compress failed, could not replace original binary; %s\n",infile,strerror(errno));
      my_rename(newfname,infile);
      close(fdin);
      fclose(of);
      unlink(ofname);
      return 0;
    }

  fclose(of);
  unlink(ofname);

  /* Set up permissions on new file */
  chown(infile,infstat.st_uid,infstat.st_gid);
  chmod(infile,infstat.st_mode);
  utb.modtime = infstat.st_mtime;
  utb.actime = infstat.st_atime;
  utime(infile,&utb);

  newsize += stub_wsize + 8;
  printf("\r%s: initial=%d, compressed=%d (%.02f:1, %.02f%%)\n",infile,oldsize,newsize,(((float)oldsize) / ((float)newsize)), ((float)newsize) * 100 / ((float)oldsize));

  return 1;
}

/*
 * Decompresses a self-extracting binary, returns nonzero if successful.
 * Writes infile~ as output file.
 */
char decompress_sfx_binary(char *infile)
{
  FILE *of;
  int npid,n;
  char *args[3];
  char ofname[256];
  int pipes[2];
  char readbuf[16384];
  char *tmp1,*tmp2;
  struct stat infstat;
  char newfname[256];
  int newsize;
  struct utimbuf utb;

  sprintf(ofname,"/tmp/_decomp%x%x",(unsigned int)time(NULL),getpid());
  sprintf(newfname,"%s~",infile);

  /* Stat the input file */
  if (stat(infile,&infstat))
    {
      printf("%s: decompress failed: can't stat\n",infile);
      return 0;
    }

  /* Can't decompress files we don't own unless we're super-user */
  if ((getuid())&&(infstat.st_uid != getuid()))
    {
      printf("%s: decompress failed: permission denied\n",infile);
      return 0;
    }

  /* Check to make sure it's an executable file */
  if ((!(infstat.st_mode & S_IXUSR))&&(!(infstat.st_mode & S_IXGRP)))
    {
      if (!(infstat.st_mode & S_IXOTH))
	{
	  printf("%s: decompress failed: not an executable file\n",infile);
	  return 0;
	}
    }
  if (S_ISDIR(infstat.st_mode))
    {
      printf("%s: decompress failed: %s is a directory\n",infile,infile);
      return 0;
    }

  /* Open output file */
  if (!(of = fopen(ofname,"w+")))
    {
      printf("%s: decompress failed: could not create %s\n",infile,ofname);
      return 0;
    }

  /* Create pipes */
  if (pipe(pipes))
    {
      printf("%s: decompress failed: could not create pipes!\n",infile);
      fclose(of);
      unlink(ofname);
      return 0;
    }

  /* Fork off and execute compressed binary in dump-decompressed-data mode */
  if (!(npid = fork()))
    {
      close(STDOUT_FILENO);
      dup2(pipes[1],STDOUT_FILENO);
      dup2(pipes[1],STDERR_FILENO);
      args[0] = infile;
      args[1] = ">d";
      args[2] = (char *)0;
      execv(infile,args);
    }
  close(pipes[1]);
  if (npid < 0)
    {
      printf("%s: decompress failed: could not fork!\n",infile);
      close(pipes[0]);
      fclose(of);
      unlink(ofname);
      return 0;
    }

  while((n = read(pipes[0],&readbuf,sizeof(readbuf))) > 0)
    {
      if (fwrite(&readbuf,1,n,of) != n)
	{
	  printf("%s: decompress failed: write error\n",infile);
	  close(pipes[0]);
	  fclose(of);
	  unlink(ofname);
	  return 0;
	}
    }

  close(pipes[0]);

  /* Look to make sure it's a real exe and not an error from the
   * self-decompressing executable */
  newsize = ftell(of);
  rewind(of);
  if ((n = fread(&readbuf,1,128,of)) < 128)
    {
      printf("%s: decompress failed: write error\n",infile);
      fclose(of);
      unlink(ofname);
      return 0;
    }
  readbuf[n+1] = '\0';
  if (strstr(readbuf,"can't extract"))
    {
      if ((tmp1 = strchr(readbuf,'\n')))
	{
	  if ((tmp2 = strchr(readbuf,':')))
	    {
	      *tmp1 = '\0';
	      printf("%s: decompress failed: extraction error%s\n",infile,tmp2);
	    }
	}
      else printf("%s: decompress failed: extraction error\n",infile);
      fclose(of);
      unlink(ofname);
      return 0;
    }

  fclose(of);

  unlink(infile);
  if (my_rename(ofname,infile))
    {
      printf("%s: decompress failed: could not replace original binary\n",infile);
      unlink(ofname);
      my_rename(newfname,infile);
      return 0;
    }

  /* Set up permissions on new file */
  chown(infile,infstat.st_uid,infstat.st_gid);
  chmod(infile,infstat.st_mode);
  utb.modtime = infstat.st_mtime;
  utb.actime = infstat.st_atime;
  utime(infile,&utb);

  printf("%s: decompress successful: compressed=%d, decompressed=%d\n",infile,(int)infstat.st_size,newsize);

  return 1;
}

/* Called for bad or nonexistant command line options */
void bad_options(void)
{
  printf("Usage: exepak [options] <files...>\n\
	   Options are:\n\
	   -b#              - Sets compression block size in Kb (default:256)\n\
	   -t               - Just test whether executables are compressed\n\
	   -d\n\
	   -x               - Decompress a compressed executable\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  int i;
  char x;
  FILE *tf;
  struct stat infstat;

  /* Initialize UCL compression library */
  if (ucl_init() != UCL_E_OK)
    {
      printf("UCL compression library failed to initialize!\n");
      exit(1);
    }

  /* Process command line options */
  if (argc < 2)
    bad_options();
  for(i=1;i<argc;i++)
    {
      if (*argv[i] == '-')
	{
	  switch(*(argv[i]+1))
	    {
	    case 'b':
	      blocksize = 1024 * atoi(argv[i]+2);
	      if (blocksize <= (1024 * 16))
		{
		  printf("Minimum blocksize is 16k\n\n");
		  bad_options();
		}
	      break;
	    case 't':
	      testing = 1;
	      break;
	    case 'x':
	    case 'd':
	      extracting = 1;
	      break;
	    default:
	      bad_options();
	    }
	}
      else
	{
	  if (testing)
	    {
	      if (stat(argv[i],&infstat))
		printf("%s: can't stat\n",argv[i]);
	      else
		{
		  if ((!(infstat.st_mode & S_IXUSR))&&(!(infstat.st_mode & S_IXGRP)))
		    {
		      if (!(infstat.st_mode & S_IXOTH))
			{
			  printf("%s: not an executable file\n",argv[i]);
			}
		    }
		  else if (S_ISDIR(infstat.st_mode))
		    {
		      printf("%s: is a directory\n",argv[i]);
		    }
		  else if ((tf = fopen(argv[i],"r")))
		    {
		      fclose(tf);
		      if ((x = check_already_compressed(argv[i])))
			printf("%s: compressed (%s)\n",argv[i],((x == 2) ? "1.1" : "1.0"));
		      else printf("%s: uncompressed\n",argv[i]);
		    }
		  else printf("%s: file not found or unreadable\n",argv[i]);
		}
	    }
	  else if (extracting)
	    {
	      if ((x = check_already_compressed(argv[i])))
		{
		  if (x == 2)
		    decompress_sfx_binary(argv[i]);
		  else printf("%s: decompress failed: this is an exepak 1.0 binary, run it\n%s  with the command line \"^dc-<target filename>\" to decompress\n",argv[i],tospaces(argv[i]));
		}
	      else printf("%s: decompress failed: not compressed\n",argv[i]);
	    }
	  else
	    {
	      if (!check_already_compressed(argv[i]))
		create_sfx_binary(argv[i]);
	      else printf("%s: compress failed: already compressed\n",argv[i]);
	    }
	}
      waitpid(-1,(int *)0,WNOHANG);
    }

  exit(0);
}
