#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
//#include "../../Manager/inc/manager.h"

#define BUFFER_SIZE 10
#define lambda 1.0

// lambda = arrival rate = 1/IAT; IAT = Inter Arrival Time
// newIAT = -log(rand())/lambda
// newIAT is calculated in us (microseconds)
int nexp() {
    double newIAT;
    double rnd;

    rnd = (double) (rand()%1000) / 1000; // rnd element of [0, 1]
    newIAT = -1000*(log(rnd)/lambda); // calculation of newIAT (-log(rnd)/lambda), in microseconds
    return (int)(newIAT * 60);
}

