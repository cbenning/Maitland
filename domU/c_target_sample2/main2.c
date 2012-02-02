#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#define NUMPAGES 300
#define PAGESIZE 4096
//getpagesize()

int main( int argc, const char* argv[] ){

	char *empty_var;
	int i;
	char buf[100];
	int tmp,pagemult;

    pagemult = atoi(argv[1]);

	empty_var = calloc(pagemult,PAGESIZE);

	for(i=0; i < pagemult; i++){
		tmp = i*i;
		sprintf(buf,"This is not a test:%d\n",(int)time(NULL));
		strcpy(empty_var+i*PAGESIZE,buf);
	}
    i = 0;

	while(i < 1){
        //printf("JAJAJA\n");
		sleep(1.0);
        i++;
	}

    return 0;

}

