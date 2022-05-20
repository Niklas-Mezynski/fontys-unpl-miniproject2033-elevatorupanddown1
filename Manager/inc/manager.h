#ifndef __MANAGER
#define __MANAGER

#include "elevator.h"

#define NO_ELEVATORS 2
#define NO_FLOORS 10
#define NO_APPS_PER_FLOOR 10

#define PORT 8080
#define MAX_CONNECTIONS 8
#define BUFFER_SIZE 1024


// Different types of messages
typedef enum
{
    m1,
    m2,
    m3
} MessageType;

// Different types of message senders
typedef enum
{
    E,
    F,
    M
} SenderType;

// Struct for sending information packets between the clients and server
struct dataPacket
{
    MessageType mType;
    SenderType sType;
    char mBuffer[BUFFER_SIZE];
};

/*
    Initializes the elevators and creates a thread for each one of them
    - Argument(s): targetElevators: an array of elevator pointers which stores the elevator structs
*/
void initialzeElevators();

#endif