#ifndef __FLOOR
#define __FLOOR

#include "LL.h"

#define QUEUE_ID 1

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
    long mtype;
    int floorID;
    int noPeople;  
} floor_to_client;

typedef struct
{
    long mtype;
    int floorID;
    //int noPeople;  
} client_to_floor;

void* start();

//initialize m floors
void initializeFloors();

//starting a loop for comunication with manager
void* floorClient();

//start producing of residents of a floor
void startFloors();

#endif