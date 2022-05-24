#ifndef __ELEVATOR
#define __ELEVATOR

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
    int nextTargetFloor;
    guest **guestsInside;
} elevator;

/*
    Starts the elevator (each thread is going to run that method)
    Loops until the day is over and waits for events
    - Argument(s): elevator: a pointer to the elevator struct of itself
*/
void *start(void *e);

#endif