#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "oss.h"

static int setUpSHM(){
	shmClockid = shmget(8837, sizeof(sysClock), IPC_CREAT | IPC_EXCL | 0666);
	if(shmClockid == -1){
		perror("oss error: shmget shmClockid");
		return -1;
	}

	sysClockptr = shmat(shmClockid, NULL, 0);
	if(sysClockptr == (void *) -1){
		perror("oss error: shmat sysClockptr");
		return -1;
	}

	return 0;
}

int main(void) {
	time_t t;
  srand((unsigned) time(&t));

	pid_t pid;

  setUpSHM();

	increaseClock(1, 1000000000);
	printf("%d:%d seconds:nanoseconds in parent\n", sysClockptr->seconds, sysClockptr->nanoseconds);

	pid = fork();
	if(pid == 0){
		execl("userProcess", "userProcess", NULL);
	} else{
		printf("this is parent\n");
	}
	
	//removeSHM();

	return 0;
}