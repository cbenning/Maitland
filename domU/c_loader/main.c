
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define DEVICE_FILE_NAME "/dev/malpage"
#define MALPAGE_IOC_MAGIC 250
#define MALPAGE_WATCH MALPAGE_IOC_MAGIC+10
#define ONE_MILLION 1000000
#define IRQ_SCRIPT "/root/getsoftirqs.sh"
#define LOGFILE_PREFIX "/root"

int main( int argc, char* argv[] ){

	int file_desc, ret_val;
    pid_t pid;
    char *command;
    int status;
	struct timeval *start_time,*end_time;
    long elapsed_sec,elapsed_usec;
	int test_mode;
    int shmkey,shmkey2,shmid;
    long start_irqs,end_irqs,total_irqs;
    void *shmarea;
    shmkey = 1234;
    shmkey2 = 1235;
    FILE *fp;
    FILE *file;
    char *start_irqs_str;
    char *end_irqs_str;
    char *logfile;

    //cwd = calloc(1024,1);
    //getcwd(cwd,1024);

	if(argc < 2){
		printf("Not enough parameters\n");
		return -1;
	}
	
	test_mode = 0;
	command = argv[1];
	if(argc >= 3 && strcmp(argv[1],"test")==0){
		test_mode = 1;
		command = argv[2];
		printf("Test Mode Enabled\n");
	}

   	file_desc = open(DEVICE_FILE_NAME,O_RDWR);
	if (file_desc < 0) {
		printf("Can't open device file\n");
		return -1;
	}

	//Attempt to fork
	if((pid = fork()) < 0){
		printf("Unable to create child process, exiting.\n");
		return -1;
	}

	//If fork was successful
	else{

        //start_time = calloc(sizeof(struct timeval),1);
        end_time = calloc(sizeof(struct timeval),1);
        end_irqs_str = calloc(1024,1);
        

		//If current thread is the child
		if (pid == 0){

            if((pid = getpid()) <0){
                printf("Unable to determine my PID\n");
                return -1;
            }

            //printf("Got PID %u\n",(unsigned int)pid);
            //run watch
			if(!test_mode){
				ret_val = ioctl(file_desc, MALPAGE_WATCH, pid);
				if (ret_val < 0) {
					printf("ioctl_msg MALPAGE_WATCH failed\n");
				}
			}
			else{
				printf("Test Mode Enabled, not setting watch\n");
			}

            if((shmid = shmget(shmkey, sizeof(struct timeval), IPC_CREAT | 0666)) < 0) {
                exit(1);
            }

            if((shmarea = shmat(shmid, NULL, 0)) == (char *) -1) {
                exit(1);
            }
            start_time = (struct timeval*)shmarea;

            if((shmid = shmget(shmkey2, 1024, IPC_CREAT | 0666)) < 0) {
                exit(1);
            }

            if((shmarea = shmat(shmid, NULL, 0)) == (char *) -1) {
                exit(1);
            }

            start_irqs_str = (char*)shmarea;
            sleep(3);

            /* Open the command for reading. */
            fp = popen(IRQ_SCRIPT, "r");
            fgets(start_irqs_str, sizeof(start_irqs_str)-1, fp);
            pclose(fp);

			gettimeofday(start_time,NULL);
			//printf("Start Time: %lu, %lu\n",start_time->tv_sec,start_time->tv_usec);
            if(test_mode){
    		    ret_val = execvp(command,&argv[2]);
            }
            else{
    		    ret_val = execvp(command,&argv[1]);
            }

			if(ret_val < 0){
				printf("%s: %s\n",strerror(errno),command);
			}
		    return 0;
		}
		//If current thread is the parent
		else{

		    wait(&status);
			gettimeofday(end_time,NULL);

            /* Open the command for reading. */

            fp = popen(IRQ_SCRIPT, "r");
            fgets(end_irqs_str, sizeof(end_irqs_str)-1, fp);
            pclose(fp);

            if((shmid = shmget(shmkey, sizeof(struct timeval), IPC_CREAT | 0666)) < 0) {
                perror("shmget");
                exit(1);
            }

            if((shmarea = shmat(shmid, NULL, 0)) == (char *) -1) {
                perror("shmat");
                exit(1);
            }
            start_time = (struct timeval*)shmarea;

            if((shmid = shmget(shmkey2, sizeof(start_irqs_str), IPC_CREAT | 0666)) < 0) {
                exit(1);
            }

            if((shmarea = shmat(shmid, NULL, 0)) == (char *) -1) {
                exit(1);
            }
            start_irqs_str = (char*)shmarea;

            elapsed_sec = end_time->tv_sec - start_time->tv_sec;
            elapsed_usec = end_time->tv_usec - start_time->tv_usec;
            if(elapsed_usec < 0){
                elapsed_sec-=1;
                elapsed_usec = ONE_MILLION+elapsed_usec;
            }

            start_irqs = atoi(start_irqs_str);
            end_irqs = atoi(end_irqs_str);
            total_irqs = end_irqs-start_irqs;

			//printf("Str Time: %lu, %lu\n",start_time->tv_sec,start_time->tv_usec);
			//printf("End Time: %lu, %lu\n",end_time->tv_sec,end_time->tv_usec);
			printf("%ld %ld.%06ld\n",total_irqs,elapsed_sec,elapsed_usec);


            logfile = NULL;
            asprintf(&logfile, "/root/data_%s", command+2);
            file = fopen(logfile,"w");
            fprintf(file,"%ld %ld.%06ld\n",total_irqs,elapsed_sec,elapsed_usec);
            fclose(file);

		}
	}

    return 0;

}
