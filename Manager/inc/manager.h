#ifndef __MANAGER
#define __MANAGER

#include "elevator.h"

#define NO_ELEVATORS 2
#define NO_FLOORS 10
#define NO_APPS_PER_FLOOR 10

#define PORT 8080
#define MAX_CONNECTIONS 8
//Buffer size for incoming messages
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

/*
    Sets everything up and starts the manager
*/
void startManager();

/*
    The thread function for the manager itself
    Loops until the day is over and waits for messages from the floors
*/
void *managerLoop();

/*
    Initializes the server socket and waits for a connection
    - Argument(s): client_socket: a pointer to the client_socket the server will connect to
*/
int initServer(int *client_socket);

static void error_exit(char *error_message);

#endif