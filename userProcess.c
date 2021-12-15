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
		perror("userProcess error: shmget shmClockid");
		return -1;
	}

	sysClockptr = shmat(shmClockid, NULL, 0);
	if(sysClockptr == (void *) -1){
		perror("userProcess error: shmat sysClockptr");
		return -1;
	}

	shmid = shmget(7000, 0, 0);
	if(shmid == -1){
		perror("userProcess error: shmget shmid");
		return -1;
	}

	shmPtr = shmat(shmid, NULL, 0);
	if(shmPtr == (void *) -1){
		perror("userProcess error: shmat shmPtr");
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

static void writeMSG(){
	sprintf(message.mesg_text, "%d", getpid());
	message.mesg_type = 1;
	msgsnd(msgid, &message, sizeof(message), 0);
}

static void initFrames(){
	time_t t;
  srand((unsigned) time(&t));

	int requestAddy;
	int offset;

	int readWrite;

	int i;

	for(i = 0; i <= 255; i++){
		offset = rand() % 1024 + 0;
		requestAddy = rand() % 33 + 0;
		requestAddy = requestAddy * 1024;
		requestAddy = requestAddy + offset;

		shmPtr->table.frames[i] = requestAddy;

		readWrite = rand() % 11 + 0;

		if (readWrite <= 7){
			shmPtr->table.refFlag[i] = 0;
		} else {
			shmPtr->table.refFlag[i] = 1;
		}
	}
}

static void readorwrite(){
	int readWrite;
	int i;

	for(i = 0; i < 18; i++){
		readWrite = rand() % 11 + 0;
		if(readWrite < 3){
		shmPtr->readORwrite[i] = 1;
		} else shmPtr->readORwrite[i] = 0;
	}
}

static int requestAddress(){
	int requestAddy;
	int offset;

	offset = rand() % 1024 + 0;
	requestAddy = rand() % 33 + 0;
	requestAddy = requestAddy * 1024;
	requestAddy = requestAddy + offset;

	return requestAddy;
}

static void initState(){
	int i;
	for(i = 0; i < MAX_PROCESS; i++){
		shmPtr->procState[i] = 0;
	}
}

static void printTable(){
	printf("Current memory layout at time %d:%d is\n", sysClockptr->seconds, sysClockptr->nanoseconds);
	printf("\t\tOccupied\t\tDirtyBit\n");
	int i;
	for(i = 0; i < 256; i++){
		printf("Frame %d:\t %d\t%d\n", i, shmPtr->table.occupy[i], shmPtr->table.dirtyBit[i]);
	}
}

static void run(){
	int i;
	int req;
	int k;
	int frameCount;
	int t = 0;
	int loglines = 0;

	for(i = 0; i < MAX_PROCESS; i++){
		increaseClock(1, 500000000);

		while(shmPtr->procState[i] == 0){
			req = requestAddress();
			if(shmPtr->readORwrite[i] == 0){
				printf("Master: P%d request read of address %d at time %d:%d\n", i, req, sysClockptr->seconds, sysClockptr->nanoseconds);
				loglines++;
				sysClockptr->nanoseconds += 10;
			} else {
					printf("Master: P%d request write of address %d at time %d:%d\n", i, req, sysClockptr->seconds, sysClockptr->nanoseconds);
					loglines++;
					sysClockptr->nanoseconds += 10;
				}

			t = 0;

			while(t == 0){
				int temp;
				for(k = 0; k < 256; k++){
					if(shmPtr->readORwrite[i] == 0){

						if(req < shmPtr->table.frames[i]){
							printf("Master: Address %d in frame %d, giving data to P%d at time %d:%d\n", req, k, i, sysClockptr->seconds, sysClockptr->nanoseconds);
							loglines++;
							frameCount = 0;

							shmPtr->table.occupy[i] = 0;
							shmPtr->table.dirtyBit[i] = 0;
							

							if ((sysClockptr->nanoseconds + 14000000) > 1000000000){
								temp = (sysClockptr->nanoseconds + 14000000) - 1000000000;
								sysClockptr->nanoseconds = temp;
								sysClockptr->seconds += 1;
							} else sysClockptr->nanoseconds += 14000000;

							t = 1;
							k = 256;

						}else {
							frameCount++;
							shmPtr->table.occupy[i] = 1;
							shmPtr->table.dirtyBit[i] = 0;
						}

					} else{
						if(req < shmPtr->table.frames[i]){
							printf("Master: Address %d in frame %d, writing data to frame at time %d:%d\n", req, k, sysClockptr->seconds, sysClockptr->nanoseconds);
							loglines++;
							frameCount = 0;

							shmPtr->table.occupy[i] = 0;
							shmPtr->table.dirtyBit[i] = 1;

							if ((sysClockptr->nanoseconds + 14000000) > 1000000000){
								temp = (sysClockptr->nanoseconds + 14000000) - 1000000000;
								sysClockptr->nanoseconds = temp;
								sysClockptr->seconds += 1;
							} else sysClockptr->nanoseconds += 14000000;

							t = 1;
							k = 256;

						} else {
							frameCount++;
							shmPtr->table.occupy[i] = 1;
							shmPtr->table.dirtyBit[i] = 1;
						}
					}
				}
				if (frameCount > 255){
					printf("Master: Address %d is not in a frame, pagefault\n", req);
					loglines++;
					t = 1;
					frameCount = 0;
					shmPtr->procState[i] = 1;
				}

				if (loglines > 50){
					printTable();
					loglines = 0;
				}
			}
		}	
	}
}

int main(const int argc, char *const argv[]){
	time_t t;
  srand((unsigned) time(&t));

	stdout = freopen("logfile.txt", "w,", stdout);
	if(stdout == NULL){
		perror("freopen failed:");
		return -1;
	}

	setUpSHM();
	setUPmsgQ();
	initState();
	initFrames();
	readorwrite();
	run();
	printTable();

	removeSHM();
	removeMsgQ();
	return 0;
}