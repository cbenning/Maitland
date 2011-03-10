#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMPAGES 50
#define MEMSIZE getpagesize()*NUMPAGES


int main( int argc, const char* argv[] ){

	char *empty_var;
	int i;
	int j;
	char buf[100];
	empty_var = calloc(NUMPAGES,getpagesize());

	for(i=0; i < NUMPAGES; i++){
		sprintf(buf,"%s:%d","This is a test! ",i);
		strcpy(&(empty_var[i*getpagesize()]),buf);
	}

	while(1){
		sleep(2);
	}

}
