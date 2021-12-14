#ifndef OSS_H
#define OSS_H

typedef struct sysClock{
	unsigned int seconds;
	unsigned int nanoseconds;
} sysClock;

struct mesg_buffer{
	long mesg_type;
    char mesg_text[100];
} message;

static sysClock *sysClockptr;
int shmClockid;

int msgid;

static void removeSHM(){
	if(shmdt(sysClockptr) == -1){
		perror("error: shmdt sysClockptr");
	}

	if(shmctl(shmClockid, IPC_RMID, NULL) == -1){
		perror("error: shmctl shmClockid");
	}
}

static void removeMsgQ(){
	if(msgctl(msgid, IPC_RMID, NULL) == -1){
		perror("error: msgctl msgid");
	}
}

void increaseClock(unsigned int min, unsigned int max){
	int timeIncrease;
	int temp;

	timeIncrease = (unsigned int) (rand() % max + min);

	if ((sysClockptr->nanoseconds + timeIncrease) > 1000000000){
		temp = (sysClockptr->nanoseconds + timeIncrease) - 1000000000;
		sysClockptr->nanoseconds = temp;
		sysClockptr->seconds += 1;
	} else sysClockptr->nanoseconds += timeIncrease;
}






#endif