#ifndef __ELEVATOR
#define __ELEVATOR

#include "LL.h"

#define MAX_PER_ELEVATOR 5

typedef struct
{
    int arpartmentFloor;
    int targetFloor;
} guest;

typedef struct
{
    int id;
    double height;
    int nextTargetFloor;    // -1 if no target
    guest **guestsInside;
} elevator;

/*
    Starts the elevator (each thread is going to run that method)
    Loops until the day is over and waits for events
    - Argument(s): elevator: a pointer to the elevator struct of itself
*/
void *start(void *e);

// Helper struct to pass multiple args to the message reciever thread function
struct messageThreadArgs
{
    elevator *thisElevator;
    LinkedList *targetFloorQueue;
    pthread_mutex_t *queue_lock;
};
/*
    Thread function that is responsible for recieving for that elevator
    - Argument(s): messageThreadArgs: Contains a struct with the args
*/
void *recieve_messages(void *messageThreadArgs);

#endif