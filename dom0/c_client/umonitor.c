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

#define DEVICE_FILE_NAME "/dev/monitor"

/* Use 'm' as magic number */
#define MONITOR_IOC_MAGIC 270

//IOCTL commands
#define MONITOR_MAPMEN_PFN MONITOR_IOC_MAGIC+1

//commandline args
#define UMONITOR_CMD_MAPMEN_PFN "map"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

//struct for linked list of PFN ids
typedef struct pfn_col{
	unsigned int process_id;
	unsigned int domiain_id;
	unsigned long *pfnlist;
	unsigned int process_age;
	unsigned int pfnlist_length;
}pfn_col;


int main( int argc, const char* argv[] ){

	if(argc != 3){
		printf("Not enough params\n");
		exit(-1);
	}
	
	int pid = atoi(argv[1]);
	char* cmd = calloc(0, 255);
	strcpy(cmd,argv[2]);
	int file_desc, ret_val;
	int tmp = 0;

	file_desc = open(DEVICE_FILE_NAME,O_RDWR);

	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
	
	if(strcmp( cmd, UMONITOR_CMD_MAPMEN_PFN )==0){
	
			//Fill the struct with data
			pfn_col pfn_col_t;
			//pfn_col_t.pid = pid;
			//pfn_col_t.did = did;
	
			ret_val = ioctl(file_desc,  MONITOR_MAPMEN_PFN, &pfn_col_t);
			if (ret_val < 0) {
				printf("ioctl_msg MONITOR_MAPMEN_PFN failed:%d\n", ret_val);
			}
	}/*
	else if(strcmp( cmd, CMD_PAGEINFO )==0){
			ret_val = ioctl(file_desc, MALPAGE_PAGEINFO, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PAGEINFO failed:%d\n", ret_val);
			}
	}*/
	else{
			printf("Command not recognized: %d\n", cmd);
	}
	
	
	close(file_desc);

	return 0;
}


