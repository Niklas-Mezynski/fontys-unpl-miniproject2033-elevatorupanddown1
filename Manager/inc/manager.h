#ifndef __MANAGER
#define __MANAGER

#include <stdbool.h>
#include <sys/msg.h>

// General case defs
#define NO_ELEVATORS 2
#define NO_FLOORS 10
#define NO_APPS_PER_FLOOR 10

// Server defs
#define PORT 8080
#define MAX_CONNECTIONS 8
#define BUFFER_SIZE 128 // Buffer size for incoming messages

// MessageQueue defs
#define QUEUE_ID_MANAGER_TO_ELEVATOR 1
#define QUEUE_ID_ELEVATOR_TO_MANAGER 2
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
    long mtype; // message type, must be > 0, contains the id + 1 of the elevatorthat the message is meant for
    int floor;  // The floor the elevator should move to next
} manager_to_elevator;

typedef struct
{
    long mtype; // message type, must be > 0, contains the id + 1 of the elevatorthat the message is meant for
    int floor;  // The floor the elevator should move to next
} elevator_to_manager;

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

/*
    Initializes the message queue with the ID defined in manager.h
    - Argument(s): target_queue_id: a pointer to target int for the queue id
*/
void initMsgQueue(int *target_queue_id, int custom_queue_id, int flag);

/*
    Checks if there are any new messages in the msg input queue
    - Argument(s):  targetqueueInfos: pointer to store queue information in
                    msg_out: pointer where the msg should be stored (if exists)
    - returns:      true if a messages has been recieved and stored
*/
bool checkIncomingMsgs(struct msqid_ds *queueInfos, elevator_to_manager *msg_out);

// Not implemented yet
void handleMessage();

/*
    Prints the error_message to the console and exits the program
*/
void error_exit(char *error_message);

double clockToMillis(clock_t timeBegin, clock_t timeEnd);

int getCurrentTicks();

#endif