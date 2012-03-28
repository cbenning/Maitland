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

#define DEVICE_FILE_NAME "/dev/malpage"

#define BUFSIZE 100000

/* Use 'm' as magic number */
#define MALPAGE_IOC_MAGIC 250

//IOCTL commands
#define MALPAGE_PRINTMSG MALPAGE_IOC_MAGIC+1
#define MALPAGE_PAGEINFO MALPAGE_IOC_MAGIC+2
#define MALPAGE_PFNLIST MALPAGE_IOC_MAGIC+3
#define MALPAGE_PFNCLR MALPAGE_IOC_MAGIC+4
#define MALPAGE_PFNCOUNT MALPAGE_IOC_MAGIC+5
#define MALPAGE_PFNLISTSHOW MALPAGE_IOC_MAGIC+6

//commandline args
#define CMD_PRINTMSG "message"
#define CMD_PAGEINFO "info"
#define CMD_PFNLIST "listmake"
#define CMD_PFNCLR "listclear"
#define CMD_PFNCOUNT "listsize"
#define CMD_PFNLISTREAD "listread"
#define CMD_PFNLISTSHOW "listshow"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main( int argc, const char* argv[] ){

	if(argc != 3){
		printf("Not enough params\n");
		exit(-1);
	}
	
	int pid = atoi(argv[1]);
	char* cmd = calloc(0, 255);
	strcpy(cmd,argv[2]);
	unsigned long buf[BUFSIZE/sizeof(unsigned long)];
	int file_desc, ret_val;
	int tmp = 0;

	file_desc = open(DEVICE_FILE_NAME,O_RDWR);

	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
	
	if(strcmp( cmd, CMD_PRINTMSG )==0){
			ret_val = ioctl(file_desc, MALPAGE_PRINTMSG, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PRINTMSG failed:%d\n", ret_val);
			}
	}
	else if(strcmp( cmd, CMD_PAGEINFO )==0){
			ret_val = ioctl(file_desc, MALPAGE_PAGEINFO, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PAGEINFO failed:%d\n", ret_val);
			}
	}
	else if(strcmp( cmd, CMD_PFNLIST )==0){
			ret_val = ioctl(file_desc, MALPAGE_PFNLIST, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PFNLIST failed:%d\n", ret_val);
			}
	}
	else if(strcmp( cmd, CMD_PFNCLR )==0){
			ret_val = ioctl(file_desc, MALPAGE_PFNCLR, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PFNCLR failed:%d\n", ret_val);
			}
	}
	else if(strcmp( cmd, CMD_PFNCOUNT )==0){
			ret_val = ioctl(file_desc, MALPAGE_PFNCOUNT, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PFNCOUNT failed:%d\n", ret_val);
			}		
	}
	else if(strcmp( cmd, CMD_PFNLISTSHOW )==0){
			ret_val = ioctl(file_desc, MALPAGE_PFNLISTSHOW, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_PFNLISTSHOW failed:%d\n", ret_val);
			}		
	}
	else if(strcmp( cmd, CMD_PFNLISTREAD )==0){
	
		ret_val = read(file_desc, buf, BUFSIZE);
		if (ret_val < 0) {
			printf("Failed reading from device:%d\n", ret_val);
			exit(-1);
		}

		while(tmp<ret_val/sizeof(unsigned long)){
			printf("%lu\n",buf[tmp]);
			tmp++;
		}	
	}
	else{
			printf("Command not recognized: %d\n", cmd);
	}
	
	
	close(file_desc);
	return 0;
}


