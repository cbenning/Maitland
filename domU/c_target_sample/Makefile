
all:
	gcc -Wall -o sample_proc main.c 
    
static:
	gcc -Wall --static -o sample_proc main.c
	upx sample_proc
	
clean:
	rm sample_proc
	
