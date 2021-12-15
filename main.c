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

	shmid = shmget(7000, sizeof(struct userProcess), IPC_CREAT | IPC_EXCL | 0666);
	if(shmid == -1){
		perror("oss error: shmget shmid");
		return -1;
	}

	shmPtr = shmat(shmid, NULL, 0);
	if(shmPtr == (void *) -1){
		perror("oss error: shmat shmPtr");
	}
	return 0;
}

static int setUPmsgQ(){
	msgid = msgget(1220, 0666 | IPC_CREAT);
	if (msgid == -1){
		perror("userProcess error: msgid");
		return -1;
	}
	return 0;
}

int main(void) {
	time_t t;
  srand((unsigned) time(&t));

	pid_t pid;

  setUpSHM();
	setUPmsgQ();

	pid = fork();
	if(pid == 0){
		execl("userProcess", "userProcess", NULL);
	} else{
	}

	return 0;
}