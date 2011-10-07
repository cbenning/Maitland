SHELL=/bin/sh
USE_ASM=yes #only the asm stub works at this time, so don't change this

ifeq ($(USE_ASM),yes)
UCL_ASM="-DUCL_USE_ASM"
else
UCL_ASM=
endif

all:	libucl
#	make -C sstrip
	make -C stub USE_ASM=$(USE_ASM) UCL_ASM=$(UCL_ASM)
	make -C src
	@echo
	@echo "   Finished compiling. You will find exepak in this directory."
	@echo "Test it and copy it wherever you want."
	@echo

debug:	libucl
#	make -C sstrip
	make -C stub USE_ASM=$(USE_ASM) UCL_ASM=$(UCL_ASM) DEBUG_STUB=yes
	make -C src debug
	@echo
	@echo "   Finished compiling. You will find exepak in this directory."
	@echo "Test it and copy it wherever you want."
	@echo

libucl:	
	@[ -f ucl/Makefile ] || { cd ucl; ./configure --disable-largefile; }
	@[ -f ucl/src/.libs/libucl.a ] || make -C ucl

clean:
	make -C ucl clean
#	make -C sstrip clean
	make -C stub clean
	make -C src clean
	rm -f exepak

distclean:
	make -C ucl distclean
#	make -C sstrip clean
	make -C stub clean
	make -C src clean
	rm -f exepak
