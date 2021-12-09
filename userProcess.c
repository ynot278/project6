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
	shmClockid = shmget(8837, 0, 0);
	if (shmClockid == -1){
		perror("userProcess: shmget shmClockid");
		return -1;
	}

	sysClockptr = shmat(shmClockid, NULL, 0);
	if(sysClockptr == (void *) -1){
		perror("oss error: shmat sysClockptr");
		return -1;
	}
}

int main(const int argc, char *const arv[]){
	time_t t;
  srand((unsigned) time(&t));

	setUpSHM();

	printf("%d:%d seconds:nanoseconds in child process\n", sysClockptr->seconds, sysClockptr->nanoseconds);

	removeSHM();
}