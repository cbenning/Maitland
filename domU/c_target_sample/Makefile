
all:
	gcc -Wall -o sample_proc main.c 
    
static:
	gcc -Wall --static -o sample_proc main.c
	cp sample_proc sample_proc_p
	upx sample_proc_p
	
clean:
	rm sample_proc
	rm sample_proc_p
	
