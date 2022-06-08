#ifndef __MANAGER
#define __MANAGER

#include <stdbool.h>
#include <sys/msg.h>
#include "elevator.h"

// General case defs
#define NO_ELEVATORS 1
#define NO_FLOORS 10
#define NO_APPS_PER_FLOOR 10

// Server defs
#define PORT 8080
#define MAX_CONNECTIONS 8
#define BUFFER_SIZE 128 // Buffer size for incoming messages

// MessageQueue defs
#define FLOOR_QUEUE_ID 187
#define MSG_QUEUE_ID 14534
#define MAX_MSG_SIZE 64
#define NUM_MESSAGES 15
#define ELEVATOR_TO_MANAGER_MTYPE 2
#define LOGGER_THREAD_MTYPE 187

// Helpers
#define MILLI_TO_MICRO 1000

typedef struct
{
    long mtype;     // message type, must be > 1, indicates the floor where someone spawned + 2
    int destFloor;  // The destination floor of that person
} manager_to_elevator;

typedef struct
{
    long mtype; // message type = ELEVATOR_TO_MANAGER_MTYPE to specify which struct to recieve
    int floor;  // The floor on which the elevator picked up someone
} elevator_to_manager;

typedef enum
{
    StartIdle,  // An elevator starts to idle
    StopIdle    // An elevator starts to move again (stop idling)
} LoggerInfo;

// Server message transfer from client to the manager
typedef struct
{
    long mtype;         // message type, must be > 0, contains the id + 1 of the floor where people are waiting
    LoggerInfo info;    // Stores the type of event the logger has to handle
    clock_t time;
    int elevator_id;
} logger_message;

// Server message transfer from client to the manager
typedef struct
{
    int floorID;
    int noPeople;
} client_to_manager;

typedef struct 
{
    int floorID;            // From which floor the elevator picks up people
    int noPeopleInElevator; // How many people the elevator picks up
} manager_to_client;


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
    - Argument(s):  msg_out: pointer where the msg should be stored (if exists)
    - returns:      true if a messages has been recieved and stored
*/
bool checkIncomingMsgs(elevator_to_manager *msg_out);

// Not implemented yet
void handleMessage();



int getCurrentTicks();

void pickupPeople(elevator *elevator, long floor);

void *loggerThread();

// --- Helpers ---

/*
    Prints the error_message to the console and exits the program
*/
void error_exit(char *error_message);

double clockToMillis(clock_t timeBegin, clock_t timeEnd);

void changeElevatorsRunningValue(int val);
bool isSimulationRunning();

#endif