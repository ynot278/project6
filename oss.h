#ifndef OSS_H
#define OSS_H

typedef struct sysClock{
	unsigned int seconds;
	unsigned int nanoseconds;
} sysClock;

sysClock *sysClockptr;
int shmClockid;

static int setUpSHM(){
	shmClockid = shmget(8837, sizeof(sysClock), IPC_CREAT | IPC_EXCL | 0666);
	if(shmClockid == -1){
		perror("error: shmget shmClockid");
		return -1;
	}

	sysClockptr = shmat(shmClockid, NULL, 0);
	if(sysClockptr == (void *) -1){
		perror("error: shmat sysClockptr");
		return -1;
	}

	return 0;
}

static void removeSHM(){
	if(shmdt(sysClockptr) == -1){
		perror("error: shmdt sysClockptr");
	}

	if(shmctl(shmClockid, IPC_RMID, NULL) == -1){
		perror("error: shmctl shmClockid");
	}
}













#endif