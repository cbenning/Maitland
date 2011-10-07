
all:
	#gcc -Wall -o runme main.c 
	gcc -Wall -o runme2 main2.c 
    
static:
	#gcc -Wall --static -o runme main.c
	gcc -Wall --static -o runme2 main2.c
	cp runme2 runme2_upx
	cp runme2 runme2_gzexe
	upx runme2_upx
	gzexe runme2_gzexe
	rm runme2_gzexe~
	
clean:
	#rm runme
	rm runme2
	#rm runme_p
	rm runme2_upx
	rm runme2_gzexe
	
