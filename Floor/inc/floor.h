#ifndef __FLOOR
#define __FLOOR

#include "LL.h"

//struct for each floor
typedef struct
{
    int id;
    int no_apartments;
} floorStruct;

struct message
{
    
};

typedef struct
{
    int id;
    floorStruct *floor;
} person;

void* start();

//initialize m floors
void initializeFloors();

//starting a loop for comunication with manager
void* floorClient();

//start producing of residents of a floor
void startFloors();

#endif