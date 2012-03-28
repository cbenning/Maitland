// Maitland: A prototype paravirtualization-based packed malware detection system for Xen virtual machines
// Copyright (C) 2011 Christopher A. Benninger

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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

