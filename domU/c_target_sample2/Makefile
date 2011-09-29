
all:
	gcc -Wall -o runme main.c 
    
static:
	gcc -Wall --static -o runme main.c
	cp runme runme_p
	upx runme_p
	
clean:
	rm runme
	rm runme_p
	
