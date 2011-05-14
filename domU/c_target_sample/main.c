#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#define NUMPAGES 300
#define PAGESIZE 4096
#define MEMSIZE PAGESIZE*NUMPAGES
//getpagesize()

int main( int argc, const char* argv[] ){

	char *empty_var;
	int i;
	int j;
	char buf[100];
	empty_var = calloc(NUMPAGES,PAGESIZE);
	//printf("\npagesize %d\n",PAGESIZE);
	int tmp;
	extern int errno;  
	void* pg_aligned;

	for(i=0; i < NUMPAGES; i++){

		tmp = i*i;
		sprintf(buf,"This is a test:%d\n",time());
		strcpy(empty_var+i*PAGESIZE,buf);

		//printf("Wrote %s to page %d\n",buf, i);
		//memcpy(&(empty_var[i*NUMPAGES]),&tmp, sizeof(tmp));
	}

	for(i=0; i <= 10; i++){
		printf("Sleeping for %d...\n",i);
		sleep(1);
	}
	printf("Trying to set block as executable...\n");

	pg_aligned = (void*)empty_var;
	pg_aligned = pg_aligned - ((unsigned long)pg_aligned % PAGESIZE);

	tmp = mprotect(pg_aligned, PAGESIZE*10, PROT_EXEC); 
	if (tmp==0) {
	    /* current code page is now writable and code from it is allowed for execution */
		printf("Success\n");
	}
	else{
		printf( "Error setting permissions for page: %s\n", strerror(errno) );	
	}


	while(1){
		sleep(0.1);
	}

}
