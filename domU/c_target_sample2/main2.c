#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#define NUMPAGES 30
#define PAGESIZE 4096
#define MEMSIZE PAGESIZE*NUMPAGES
//getpagesize()

int main( int argc, const char* argv[] ){

	char *empty_var;
	int i;
	char buf[100];
	int tmp;

	empty_var = calloc(NUMPAGES,PAGESIZE);

	for(i=0; i < NUMPAGES; i++){
		tmp = i*i;
		sprintf(buf,"This is not a test:%d\n",(int)time(NULL));
		strcpy(empty_var+i*PAGESIZE,buf);
	}

	while(1){
        printf("JAJAJA\n");
		sleep(1.0);
	}

    return 0;

}

