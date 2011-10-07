#stub.s - the assembler version of the stub with no library dependencies
#	  and so much smaller ;-)

#Copyright (C) 2004-2005 by Stefan Talpalaru <stefantalpalaru@yahoo.com>

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License version 2 as published by
#the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#MA  02111-1307  USA

.arch i386

.equ SYS_FORK, 2
.equ SYS_READ, 3
.equ SYS_WRITE, 4
.equ SYS_OPEN, 5
.equ SYS_CLOSE, 6
.equ SYS_WAITPID, 7
.equ SYS_UNLINK, 10
.equ SYS_EXECVE, 11
.equ SYS_LSEEK, 19
.equ SYS_GETPID, 20
.equ SYS_MKDIR, 39
.equ SYS_RMDIR, 40
.equ SYS_BRK, 45
.equ SYS_NEWSTAT, 106
.equ SYS_NANOSLEEP, 162

.global _start
_start:
#move the start of the compressed file and zero the remaining region
movl $mapcompressed , %ecx
subl $bss_begin, %ecx # zbufsz
movl %ecx, %ebx # that's the part that gets zeroed
subl $8, %ecx # zbufsz - 8  => that's the part hat gets moved
movl $bss_begin + 8, %esi #get past complen and mode
movl %esi, %edi
addl bss_begin, %edi #%edi = bss_begin + 4 + 4 + complen
#cld
rep movsb

save_mode:
movl bss_begin + 4, %edx #save the mode

movl $bss_begin, %eax
zero_it:
movb %cl, (%eax) # %ecx is 0 at this point
incl %eax
decl %ebx
jnz zero_it

movl %edx, MODE

movl %esp, %ebp

check_args:
#xorl %ebx, %ebx #DONTSPAWN value
movl (%esp), %eax #argc
decl %eax
jz open_tmp
movl 8(%esp), %eax #argv[1]

#check if argv[1]==">d"
cmpw $0x643e, (%eax)
jne open_tmp
cmpb $0, 2(%eax)
jne open_tmp
incl %ebx # =1
movl %ebx, DONTSPAWN # store it

open_tmp:
decl %ebx #if =1 outputting to stdout
jz allocate_mem

#make tmpdir

#open rand
movl $SYS_OPEN, %eax
movl $urandom, %ebx
#xorl %ecx, %ecx #O_RDONLY
movl $0400, %edx
int $0x80
movl %eax, UFD
incl %eax
jz err

read_rand:
movl $SYS_READ, %eax
movl UFD, %ebx
movl $RND, %ecx
movl $4, %edx
int $0x80

movl $charlist, %esi
movl $DDNAME_RAND, %edi
xorl %ecx, %ecx
movl RND, %eax

write_rand:
xorl %edx, %edx
movl $7, %ebx
divl %ebx
movb (%esi, %edx), %bl
movb %bl, (%edi, %ecx)
incl %ecx
cmpl $RAND_LEN, %ecx #number of random characters
jne write_rand

make_tmpdir:
movl $SYS_MKDIR, %eax
movl $ddname, %ebx
movl $0700, %ecx
int $0x80
testl %eax, %eax
jz write_tmpf
movl tries, %eax
decl %eax
jz err
movl %eax, tries
jmp read_rand

write_tmpf:

#copy ddname to dfname
movl $DDNAME_LEN, %ecx
movl $ddname, %esi
movl $dfname, %edi
cld
rep movsb

#find the first char after the last '/' in argv[0]
movl 4(%ebp), %esi
xorl %ecx, %ecx
find_argv0_b:
cmpb $0, (%esi, %ecx)
je find_argv0_e #end of string
cmpb $'/', (%esi, %ecx)
jne find_argv0_c #continue
leal 1(%esi, %ecx), %esi #after '/'
xorl %ecx, %ecx
decl %ecx
find_argv0_c:
incl %ecx
jmp find_argv0_b
find_argv0_e:

#write basename(argv[0]) to dfname
cld
rep movsb

open_tmpf:
movl $SYS_OPEN, %eax
movl $dfname, %ebx
movl $0301, %ecx #wronly, creat, excl
movl MODE, %edx #mode
int $0x80
movl %eax, DFD #store the fd
incl %eax #-1 on error
jnz allocate_mem
movl $SYS_RMDIR, %eax
movl $ddname, %ebx
int $0x80
movl tries, %eax
decl %eax
jz err
movl %eax, tries
jmp read_rand

allocate_mem:
movl $SYS_BRK, %eax
xorl %ebx, %ebx
int $0x80
incl %eax
movl %eax, WRITEBUF_P #first unused mem. location from the heap
addl mapcompressed + 4, %eax #writebuf size
movl %eax, %ebx
movl $SYS_BRK, %eax
int $0x80
testl %eax, %eax
jz error

#set the pointer
movl $mapcompressed, %eax
movl %eax, READBUF_P

decompress_loop:

#copy sizes
# %eax contains READBUF_P, both before entering the loop, and at the end of it
movl (%eax), %ebx
testl %ebx, %ebx
jz decompress_loop_end
movl %ebx, SIZES_0
movl 4(%eax), %ecx
movl %ecx, SIZES_1

#get past them
addl $8, %eax
movl %eax, READBUF_P

cmpl %ebx, %ecx #sizes
je not_compressed

#this decompression routine (ucl_nrv2e_decompress_asm_small_8) is
#Copyright (C) 1996-2003 Markus Franz Xaver Johannes Oberhumer
#with (very) small modifications by me

ucl_nrv2e_decompress_asm_small_8:

pushl %ebp

cld
movl READBUF_P,%esi
movl WRITEBUF_P,%edi
orl $-1,%ebp
xorl %ecx,%ecx
stc
jmp decompr_start

decompr_literal: 
movsb

decompr_loop: 
addb %bl,%bl
jnz d_gotbit

decompr_start: 
movb (%esi),%bl
incl %esi
adcb %bl,%bl

d_gotbit:
jc decompr_literal

decompr_match: 
xorl %eax,%eax
incl %eax

decompr_match..15.loop: 
addb %bl,%bl
jnz decompr_match..16.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..16.gotbit: 
adcl %eax,%eax
addb %bl,%bl
jnz decompr_match..22.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..22.gotbit: 
jc decompr_match..15.break
decl %eax
addb %bl,%bl
jnz decompr_match..27.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..27.gotbit: 
adcl %eax,%eax
jmp decompr_match..15.loop

decompr_match..15.break: 
subl $3,%eax
jc decompr_match.decompr_same_off
shll $8,%eax
lodsb
xorl $-1,%eax
jz decompr_end
sarl %eax
movl %eax,%ebp
jnc decompr_match.decompr_got_off

decompr_match.decompr_mlen1: 
addb %bl,%bl
jnz decompr_match..34.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..34.gotbit: 
adcl %ecx,%ecx
jmp decompr_match.decompr_got_len

decompr_match.decompr_same_off: 
addb %bl,%bl
jnz decompr_match..39.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..39.gotbit: 
jc decompr_match.decompr_mlen1

decompr_match.decompr_got_off: 
incl %ecx
addb %bl,%bl
jnz decompr_match..44.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..44.gotbit: 
jc decompr_match.decompr_mlen1

decompr_match..49.loop: 
addb %bl,%bl
jnz decompr_match..50.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..50.gotbit: 
adcl %ecx,%ecx
addb %bl,%bl
jnz decompr_match..56.gotbit
movb (%esi),%bl
incl %esi
adcb %bl,%bl

decompr_match..56.gotbit: 
jnc decompr_match..49.loop
addl $2,%ecx

decompr_match.decompr_got_len: 
cmpl $-0x500, %ebp
adcl $2,%ecx
pushl %esi
leal (%edi,%ebp),%esi
rep movsb
popl %esi
jmp decompr_loop

decompr_end: 
movl %esp,%ebx
movl READBUF_P,%edx
addl SIZES_0,%edx
cmpl %edx,%esi
je decompr_end.1
decl %eax
movb $0x37,%al
ja decompr_end.1
movb $0x33,%al

decompr_end.1: 
subl WRITEBUF_P,%edi
movl %edi,DESTLEN

popl %ebp
ucl_nrv2e_decompress_asm_small_8_end:

testl %eax, %eax
jnz error
movl SIZES_1, %eax
cmpl DESTLEN, %eax
jne error
movl WRITEBUF_P, %ecx
jmp cont #get past the not_compressed stuff

not_compressed:
movl READBUF_P, %ecx

cont:
movl SIZES_1, %edx
movl $SYS_WRITE, %eax

cmpl $0, DONTSPAWN
je write_to_file

xorl %ebx, %ebx
incl %ebx #stdout
jmp check

write_to_file:
movl DFD, %ebx

check:
int $0x80 #sys_write

cmpl %edx, %eax
jne error

movl READBUF_P, %eax
addl SIZES_0, %eax
movl %eax, READBUF_P
jmp decompress_loop

decompress_loop_end:

#close dfd
movl $SYS_CLOSE, %eax
movl DFD, %ebx
int $0x80

cmpl $0, DONTSPAWN
jne exit

#fork twice
fork:
movl $SYS_FORK, %eax
int $0x80
testl %eax, %eax
jnz wait

movl $SYS_FORK, %eax
int $0x80
testl %eax, %eax
jnz exit

#sleep(1)
movl $SYS_NANOSLEEP, %eax
movl $timespec, %ebx
xorl %ecx, %ecx
int $0x80

#unlink
movl $SYS_UNLINK, %eax
movl $dfname, %ebx
int $0x80

#rmdir
movl $SYS_RMDIR, %eax
movl $ddname, %ebx
int $0x80

jmp exit

wait:
movl $SYS_WAITPID, %eax
xorl %ebx, %ebx
decl %ebx # -1
xorl %ecx, %ecx
xorl %edx, %edx
int $0x80

execve:
#execve
movl $SYS_EXECVE, %eax
movl $dfname, %ebx #file name
leal 4(%ebp), %ecx #argv
movl (%ebp), %esi #argc
leal 8(%ebp, %esi, 4), %edx #environment = %ebp + (argc * 4) + 8
int $0x80

error:
movl $SYS_UNLINK, %eax
movl $dfname, %ebx
int $0x80
movl $SYS_RMDIR, %eax
movl $ddname, %ebx
int $0x80
err:
movl $SYS_WRITE, %eax
movl $2, %ebx
movl $extraction_error, %ecx
movl $EXTRACTION_ERROR_SIZE, %edx
int $0x80
exit:
xorl %eax, %eax
incl %eax
int $0x80

######################
#
.data

#marker
.ascii "EPK11\0"
ddname:
.ascii "/tmp/xpk"
.equ DDNAME_RAND, . #start of the random characters
.ascii "12345678/\0" #this will be replaced by 8 random chars
.equ DDNAME_LEN, . - ddname - 1
.equ RAND_LEN, . - DDNAME_RAND - 2 #number of random characters
extraction_error:
.ascii "exepak error\n"
.equ EXTRACTION_ERROR_SIZE, . - extraction_error
.equ EXTRACTION_ERROR_CODE, extraction_error + EXTRACTION_ERROR_SIZE - 1
timespec:
.long 1
.long 0
charlist:
.ascii "abcdefg"
urandom:
.ascii "/dev/urandom\0"
tries:
.long 5


######################
#
.bss

.equ bss_begin, __bss_start
.lcomm DFD, 4
.lcomm READBUF_P, 4
.lcomm WRITEBUF_P, 4
.lcomm SIZES_0, 4 #compressed block size
.lcomm SIZES_1, 4 #uncompressed block size
.lcomm DESTLEN, 4
.lcomm DONTSPAWN, 4
.lcomm dfname, 274
.lcomm UFD, 4 # urandom fd
.lcomm RND, 4 # random number
.lcomm MODE, 4


#this is the last location
.equ mapcompressed, _end
