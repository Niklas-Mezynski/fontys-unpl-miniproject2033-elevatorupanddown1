#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include "floor.h"
#include "random.c"

#define NO_FLOORS 3
#define NO_APARTMENTS 3

floorStruct *floors[NO_FLOORS];
pthread_t threads[NO_FLOORS];

person *persons[NO_APARTMENTS];

pthread_t clientThread;

void startFloor() {
    pthread_create(&clientThread, NULL, floorClient, NULL);

    initializeFloors();

    pthread_join(clientThread, NULL);
}

void initializeFloors() 
{
    int i;
    for (i = 0; i < NO_FLOORS; i++) 
    {
        //create floors
        floors[i] = (floorStruct*)malloc(sizeof(floor));
        if(i == 0) 
        {
            floors[i] -> no_apartments = 0;
            floors[i] -> id = i;
            pthread_create(&threads[i], NULL, start, floors[i]);
        } else {
            floors[i] -> no_apartments = NO_APARTMENTS;
            floors[i] -> id = i;
            pthread_create(&threads[i], NULL, start, floors[i]);
        }
        
    }
    for (i = 0; i < NO_FLOORS; i++) 
    {
        pthread_join(threads[i], NULL);
    }

}

void* floorClient() {
    
}

void* start(void* floorArguments) {
    floorStruct* floor = (floorStruct* )floorArguments;
    LinkedList *peopleInQueue = (LinkedList* )malloc(sizeof(LinkedList));
    constructLL(peopleInQueue);
    while(1){
        sleep(nexp());
        //addRearLL()
    }
    

}
