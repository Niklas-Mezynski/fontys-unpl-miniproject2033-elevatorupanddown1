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
    guest** guestsInside;
} elevator;

void *start(void* e);

#endif