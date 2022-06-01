#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include "floor.h"

#define NO_FLOORS 3

floorStruct *floors[NO_FLOORS];
pthread_t threads[NO_FLOORS];

void initializeFloors(int noApartments) 
{
    int i;
    for (i = 0; i < NO_FLOORS; i++) 
    {
        //create floors
        floors[i] = (floorStruct*)malloc(sizeof(floor));
        floors[i] -> no_apartments = NO_FLOORS;
        pthread_create(&threads[i], NULL, start, floors[i]);
    }
    for (i = 0; i < NO_FLOORS; i++) 
    {
        pthread_join(threads[i], NULL);
    }

}

void* start() {

}
