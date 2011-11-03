#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char* argv[] ){

	struct timeval start_time;
	gettimeofday(&start_time,0);
	printf("%lu %lu\n",start_time.tv_sec,start_time.tv_usec);
	return 0;

}
