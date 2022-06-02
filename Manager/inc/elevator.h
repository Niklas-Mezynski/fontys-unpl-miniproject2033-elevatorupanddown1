#ifndef __ELEVATOR
#define __ELEVATOR

#include "LL.h"

#define MAX_PER_ELEVATOR 5
#define ELEVATOR_SPEED 3 // meter per milliseconds 

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
    pthread_t movement;
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

/*
    This function moves the elevator up depending on the elevator speed and the last position update
    - Argument(s):  thisElevator: Pointer to the elevator struct itself
                    last_move: clock time of the last position update
*/
void moveElevatorAlt(elevator *thisElevator, clock_t *last_move);

#endif