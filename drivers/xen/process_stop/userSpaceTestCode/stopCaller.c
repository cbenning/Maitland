
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define CMD1 901

#define __KERNEL__
#include <linux/sched.h>
#include <sched.h>
#include <linux/kernel.h>
#include <sys/time.h>
#include <sys/resource.h>

int main (int argc, char *argv[]) {

//argc--;
//while (argc >= 0) {
//	printf("%s\n", argv[argc--]);
//}
//printf("*********\n");
/*
sigset_t intmask;
if((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, SIGHUP) ==  -1) || (sigaddset(&intmask, SIGINT) ==  -1) || (sigaddset(&intmask, SIGURG) ==  -1)|| (sigaddset(&intmask, SIGSTOP) ==  -1)|| (sigaddset(&intmask, SIGQUIT) ==  -1)|| (sigaddset(&intmask, SIGXCPU) ==  -1)) {
	perror("Failed to initialize the signal mask");
	return 1;
}*/





printf("this is the process to be stopped %s\n", argv[argc - 1]);


int fd = open("/dev/process_stop", "r");
/*
int i ;
for (i = 1; i != 0; i++) {

	if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1)
		break;
	
	printf("we are here %i\n", i);
} */
unsigned long send;
unsigned int cmd;
send = (unsigned)atoi(argv[argc - 1]);
send = (unsigned)atoi(argv[argc - 1]);
cmd = (unsigned)atoi(argv[argc-2]);
cmd = (unsigned)atoi(argv[argc-2]);

;;;;
//printf("%i\n",(send = (unsigned)atoi(argv[argc - 1])));
//printf("my userID= %ld\n", (long)getuid());
//printf("my effective userID= %ld\n", (long)geteuid());
printf("return value from ioctl >%i<\n",ioctl(fd, cmd, send )); 
//char * here;
//here = malloc(6*sizeof(char));
//memset(here,65,6);
//printf("%i\n", read(fd,here,5));
//printf(">%s<\n", here);
/* Close the file descriptor. */
close (fd);


return 0;
}

