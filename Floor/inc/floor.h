#ifndef __FLOOR
#define __FLOOR

#include "LL.h"

//struct for each floor
typedef struct
{
    int floorID;
    int no_apartments;
} floorStruct;

struct message
{
    
};

// typedef struct
// {
//     int id;
//     floorStruct *floor;
// } person;

//struct to tell the manager how many people there are and at which floor  
typedef struct
{
    int waitingPeople;
    int floorID;  
} floor_to_manager;

void* start();

//initialize m floors
void initializeFloors();

//starting a loop for comunication with manager
void* floorClient();

//start producing of residents of a floor
void startFloors();

#endif