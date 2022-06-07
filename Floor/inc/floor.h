#ifndef __FLOOR
#define __FLOOR

#include "LL.h"

#define QUEUE_ID 1
#define MILLI_TO_MICRO 1000

//struct for each floor
typedef struct
{
    int floorID;
    int no_apartments;
} floorStruct;

typedef struct 
{
    int floorID;
    int noPeople;
} client_to_manager;

typedef struct 
{
    int floorID;
    int noPeopleInElevator;
} manager_to_client;

// typedef struct
// {
//     int id;
//     floorStruct *floor;
// } person;

//struct to tell the client how many people there are and at which floor  
typedef struct
{
    long mtype;
    int floorID;
    //int noPeople;  
} floor_to_client;

typedef struct
{
    long mtype;
    int floorID;
    //int noPeople;  
} client_to_floor;

typedef struct
{
    long mtype;
    int floorID;
    int noPeople;  
} msgFromManagerToFloor;

void* floorClient();

void* start();

//initialize m floors
void initializeFloors();

//starting a loop for comunication with manager
void* floorServer();

void* floorClient();

//init socket and build connection
void initializeSocket();

//start producing of residents of a floor
void startFloors();

double clockToMillis(clock_t timeBegin, clock_t timeEnd);

#endif