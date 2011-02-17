
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define __KERNEL__
#include <linux/sched.h>

int main () {



sigset_t intmask;
if((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, SIGHUP) ==  -1) || (sigaddset(&intmask, SIGINT) ==  -1) || (sigaddset(&intmask, SIGURG) ==  -1)|| (sigaddset(&intmask, SIGSTOP) ==  -1)|| (sigaddset(&intmask, SIGQUIT) ==  -1)|| (sigaddset(&intmask, SIGXCPU) ==  -1)) {
	perror("Failed to initialize the signal mask");
	return 1;
}








int fd = open("/dev/process_stop", "r");

int i ;
for (;;i++) {

	if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1)
		break;
	
//	printf("we are here %i\n", i);
} /**/

printf("my userID= %ld\n", (long)getuid());
printf("my effective userID= %ld\n", (long)geteuid());
printf("%i\n",ioctl (fd, 500)); 
char * here;
here = malloc(6*sizeof(char));
memset(here,65,6);
printf("%i\n", read(fd,here,5));
printf(">%s<\n", here);
/* Close the file descriptor. */
close (fd);
return 0;
}

