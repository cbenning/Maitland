
all:
	gcc -Wall -o runme main.c 
	gcc -Wall -o runme2 main2.c 
    
static:
	gcc -Wall --static -o runme main.c
	gcc -Wall --static -o runme2 main2.c
	cp runme runme_p
	cp runme2 runme2_p
	upx runme_p
	upx runme2_p
	
clean:
	rm runme
	rm runme2
	rm runme_p
	rm runme2_p
	
