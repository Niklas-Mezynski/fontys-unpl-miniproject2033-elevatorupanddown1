#ifndef __MANAGER
#define __MANAGER

#include "elevator.h"

// General case defs
#define NO_ELEVATORS 2
#define NO_FLOORS 10
#define NO_APPS_PER_FLOOR 10

// Server defs
#define PORT 8080
#define MAX_CONNECTIONS 8
#define BUFFER_SIZE 128 // Buffer size for incoming messages

// MessageQueue defs
#define QUEUE_ID 187
#define MAX_MSG_SIZE 64
#define NUM_MESSAGES 15

// Different types of messages
typedef enum
{
    m1,
    m2
    //...
} MessageType;

// Struct for sending information packets between the clients and server
typedef struct
{
    MessageType mType;
    char mBuffer[BUFFER_SIZE];
} dataPacket;

typedef struct
{
    long mtype;       /* message type, must be > 0 */
    int floor;    /* message data */
    char mtext[MAX_MSG_SIZE];    /* message data */
} messageStruct;

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

void initMsgQueue();

void handleMessage();

static void error_exit(char *error_message);

#endif