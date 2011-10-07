# Makefile for EXEPAK stub
# (c)1997 Adam Ierymenko
# (c)2004-2005 Stefan Talpalaru

CC=gcc
CFLAGS+=-Os
DEBUG_STUB=no

#CFLAGS=-g

all: exepak_stubencode

ifeq ($(USE_ASM),yes)
exepak_stub:	stub.s
	as stub.s -o stub.o
ifeq ($(DEBUG_STUB),no)
	ld -s stub.o -o exepak_stub
#	@../sstrip/sstrip exepak_stub
else
	ld -g stub.o -o exepak_stub
endif
#	@echo '.equ EXEPAK_STUBSIZE,' `ls -ls exepak_stub | awk '{print $$6}'` > stubsize.s
	@echo '#define EXEPAK_STUBSIZE' `ls -ls exepak_stub | awk '{print $$6}'` > stubsize.h
#	as stub.s -o stub.o
ifeq ($(DEBUG_STUB),no)
#	ld -s stub.o -o exepak_stub
#	@cp exepak_stub exepak_stub_unsstriped
#	../sstrip/sstrip exepak_stub
else
#	ld -g stub.o -o exepak_stub
endif


else #no asm
exepak_stub:	stub.c
	$(CC) $(CFLAGS) -I../ucl/include $(UCL_ASM) -o exepak_stub stub.c ../ucl/src/.libs/libucl.a
#	@../sstrip/sstrip exepak_stub
	@echo '#define EXEPAK_STUBSIZE' `ls -ls exepak_stub | awk '{print $$6}'` > stubsize.h
	$(CC) $(CFLAGS) -I../ucl/include $(UCL_ASM) -o exepak_stub stub.c ../ucl/src/.libs/libucl.a
	@cp exepak_stub exepak_stub_unsstriped
#	@../sstrip/sstrip exepak_stub
endif

exepak_stubencode:	exepak_stub
	$(CC) $(CFLAGS) -o exepak_stubencode stubencode.c
	./exepak_stubencode

clean:
	rm -f exepak_stub* __stub.h stub.o
