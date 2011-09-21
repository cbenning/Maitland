#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define DEVICE_FILE_NAME "/dev/malpage"
#define MALPAGE_IOC_MAGIC 250
#define MALPAGE_WATCH MALPAGE_IOC_MAGIC+10

int main( int argc, char* argv[] ){

	int file_desc, ret_val;
    pid_t pid;
    char *command;
    int status;

	if(argc < 2){
		printf("Not enough params\n");
		exit(-1);
	}

    command = argv[0];

   	file_desc = open(DEVICE_FILE_NAME,O_RDWR);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}

	//Attempt to fork
	if((pid = fork()) < 0){
		printf("Unable to create child process, exiting.\n");
		exit(-1);
	}
	//If fork was successful
	else{
		//If current thread is the child
		if (pid == 0){

            //run watch
			ret_val = ioctl(file_desc, MALPAGE_WATCH, pid);
			if (ret_val < 0) {
				printf("ioctl_msg MALPAGE_WATCH failed:%d\n", ret_val);
			}

            //run the program
		    ret_val = execvp(command,&argv[2]);
			
			if(ret_val < 0){
				printf("%s\n",strerror(errno));
			}

			exit(0);
		}
		//If current thread is the parent
		else{
		      wait(&status);
		}
	}

    return 0;

}
