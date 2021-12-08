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

#include "oss.h"

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

int main(void) {
  setUpSHM();

	increaseClock(1, 1000000000);
	printf("%d:%d seconds:nanoseconds\n", sysClockptr->seconds, sysClockptr->nanoseconds);
	
	removeSHM();

	return 0;
}