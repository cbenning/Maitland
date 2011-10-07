# Makefile for EXEPAK

CC=gcc
STRIP=strip --strip-all

all:	exepak

exepak:	exepak.c ../stub/__stub.h
	$(CC) $(CFLAGS) $(LDFLAGS) -I../ucl/include -o exepak exepak.c ../ucl/src/.libs/libucl.a
	@$(STRIP) exepak
	@cp -a exepak ..

debug:	exepak.c ../stub/__stub.h
	$(CC) -g -I../ucl/include -o exepak exepak.c ../ucl/src/.libs/libucl.a
	@cp -a exepak ..

clean:
	rm -f exepak
