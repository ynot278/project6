#ifndef OSS_H
#define OSS_H

#define MAX_PROCESS 18

struct frameTable{
	int frames[256];
	int refFlag[256];
	int dirtyBit[256];
	int occupy[256];
};

struct userProcess{
	int PIDcount[18];
	int processNum[18];
	int processAddy[18];
	int readORwrite[18];
	int processCount[18];
	int procState[18];

	struct frameTable table;
} ;

int PID;

int runningProccess = 0;
int PIDmain[18];
int pitchforksMade = 0;
int forkTimer[18];

void randomForkTimer(){
	int i;
	for(i = 0; i < 18; i++){
		forkTimer[i] = (rand() % 500000000 + 1);
	}
}

static struct userProcess *shmPtr;
int shmid;

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

	if(shmdt(shmPtr) == -1){
		perror("error: shmdt shmPtr");
	}

	if(shmctl(shmid, IPC_RMID, NULL) == -1){
		perror("error: shmctl shmid");
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