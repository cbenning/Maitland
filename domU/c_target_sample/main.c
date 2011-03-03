#include <stdio.h>
#include <stdlib.h>

#define NUMPAGES 50
#define MEMSIZE getpagesize()*NUMPAGES


int main( int argc, const char* argv[] ){

	int *empty_var;
	int i;
	empty_var = calloc(NUMPAGES,getpagesize());

	for(i=0; i < NUMPAGES; i++){
		*(empty_var+(i*NUMPAGES)) = i;
	}

	while(1){
		sleep(2);
	}

}
