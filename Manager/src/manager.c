#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "manager.h"
#include "elevator.h"

clock_t start_t;
elevator *elevators[NO_ELEVATORS];
pthread_t elevator_threads[NO_ELEVATORS];
pthread_t manager_thread;
int floor_queue_id, msg_queue_id;
int peoplePerFloor[NO_FLOORS] = {0};

int clearMsgQueues();

void startManager()
{
    // Initialize a message queue to store target floors for the elevators
    initMsgQueue(&floor_queue_id, FLOOR_QUEUE_ID, IPC_CREAT | 0600);
    // Initialize a message queue to store target floors for the elevators
    initMsgQueue(&msg_queue_id, MSG_QUEUE_ID, IPC_CREAT | 0600);
    clearMsgQueues();

    // Init the general elevator variables
    initElevatorsGeneral();

    // Start the elevators (and it's threads)
    initialzeElevators();

    // Safe the program start
    start_t = clock();

    // Start the manager thread
    pthread_create(&manager_thread, NULL, managerLoop, NULL);

    // Wait for the elevator threads to finish
    for (int i = 0; i < NO_ELEVATORS; i++)
    {
        pthread_join(elevator_threads[i], NULL);
    }
    // Wait for the manager thread
    pthread_join(manager_thread, NULL);
    // Start counting clock ticks
}

void *managerLoop()
{
    /*
    // For the client connection
    int client_socket;
    ssize_t recv_size;
    // For mesages recieved from the client socket sent by the floors
    client_to_manager *rec_msg_floor = (client_to_manager *)malloc(sizeof(client_to_manager));
    // For mesages recieved from the message queue sent by the elevator
    elevator_to_manager *rec_msg_elevator = (elevator_to_manager *)malloc(sizeof(elevator_to_manager));
    // For mesages that need to be send to the elevator over the msg_queue
    manager_to_elevator *send_msg_elevator = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));

    // Start the server and recieve a client connection
    initServer(&client_socket);

    // Manager loop -> loops until the day is over and constantly handles the message traffic between floors and elevators
    while (1)
    {
        // Recieve data from the floor socket
        if ((recv_size = recv(client_socket, rec_msg_floor, sizeof(client_to_manager), 0)) < 0)
            error_exit("Message recieve error");

        // Do something with the data (for now, just print it)
        if (recv_size > 0)
        {
            printf("Client message: %d\t%d\n", rec_msg_floor->floorID, rec_msg_floor->noPeople);
            // Put the new target in the msg_queue so the elevators know they have to pick up people from there
            send_msg_elevator->mtype = rec_msg_floor->floorID;
            peoplePerFloor[rec_msg_floor->floorID] += rec_msg_floor->noPeople;
            if (msgsnd(floor_queue_id, send_msg_elevator, sizeof(manager_to_elevator), 0) < 0)
            {
                error_exit("Manager: Cannot send message to elevator msg_queue");
            }
        }

        // Check for incoming messages by the elevator
        if (checkIncomingMsgs(rec_msg_elevator))
        {
            // Do something with the message (send it to the floor)
            // TODO
        }
    }

    // Close the connection
    close(client_socket);
    free(rec_msg_floor);
    free(rec_msg_elevator);
    return EXIT_SUCCESS;
    */

    // /*  For testing/debugging -> randomly send messages to the elevators

    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    elevator_to_manager *rec_msg_elevator = (elevator_to_manager *)malloc(sizeof(elevator_to_manager));

    while (1)
    {
        usleep((rand() % 100 + 1000) * MILLI_TO_MICRO);
        // Check for incoming messages by the
        if (checkIncomingMsgs(rec_msg_elevator))
        {
            // Do something with the message (send it to the floor)
            // TODO
        }

        // if (rand() % 10 != 0)
        //     continue;
        // Sometimes, randomly spawn a person on a random floor
        msg->mtype = (rand() % NO_FLOORS) + 1;
        peoplePerFloor[msg->mtype - 1]++;
        printf("People spawning at floor %ld \t\t\t\t%f\n", msg->mtype - 1, clockToMillis(0, clock()));
        if (msgsnd(floor_queue_id, msg, sizeof(manager_to_elevator), 0) == -1)
        {
            error_exit("Message send error");
            // perror("Message send error");
            // exit(1);
        }
    }
    free(msg);
    // */
}

bool checkIncomingMsgs(elevator_to_manager *msg_out)
{

    // IPC_NOWAIT returns an error if there are no messages
    if (msgrcv(floor_queue_id, msg_out, sizeof(elevator_to_manager), -1, IPC_NOWAIT) == -1)
    {
        return false;
    }
    return true;
}

void initialzeElevators()
{
    // Create the elevator structs and store them in the elevators array
    for (int i = 0; i < NO_ELEVATORS; i++)
    {
        elevators[i] = (elevator *)malloc(sizeof(elevator));
        elevators[i]->id = i;
        elevators[i]->height = 0;                                                        // Starting floor is always 0
        elevators[i]->nextTargetFloor = -1;                                              // No target
        elevators[i]->guestsInside = (guest **)malloc(MAX_PER_ELEVATOR * sizeof(guest)); // Allocate memory for the array of guests inside

        // Create the threads
        pthread_create(&elevator_threads[i], NULL, start, elevators[i]);
    }
}

int initServer(int *client_socket)
{
    struct sockaddr_in server, client;
    int sock;
    // Create the socket
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        error_exit("Error, could not create the socket");

    // Set the socket address information for the server
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind socket to the server address
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        error_exit("Cannot bind the socket");

    // Listen for client connections
    if (listen(sock, MAX_CONNECTIONS) == -1)
        error_exit("Socket listening error");

    printf("Server ready, waiting for connections...\n");

    unsigned int len;
    // Accept a new client connection
    len = sizeof(client);
    *client_socket = accept(sock, (struct sockaddr *)&client, &len);
    if (*client_socket < 0)
        error_exit("Accept error");
    printf("Connected with client address: %s\n", inet_ntoa(client.sin_addr));

    return EXIT_SUCCESS;
}

void initMsgQueue(int *target_queue_id, int custom_queue_id, int flag)
{
    // create a public message queue, with access only to the owning user
    // *target_queue_id = msgget(custom_queue_id, IPC_CREAT | 0600);
    *target_queue_id = msgget(custom_queue_id, flag);
    if (floor_queue_id == -1)
    {
        error_exit("Manager: error creating the msg queue with msgget");
    }
}

void pickupPeople(elevator *elevator, int floor)
{
    if (peoplePerFloor[floor] < 1)
    {
        printf("pickupPeople error: there are no people on that floor (%d)\n", floor);
        exit(EXIT_FAILURE);
    }
    printf("Elevator %d is now picking up people from floor %d\t%f\n", elevator->id, floor, clockToMillis(0, clock()));

    // Pick up the guy
    peoplePerFloor[floor]--;
    elevator_to_manager *msg_to_manager = (elevator_to_manager *)malloc(sizeof(elevator_to_manager));
    msg_to_manager->mtype = 1; // 1 for msg_to_manager
    msg_to_manager->floor = floor;
    // Send the event to the msg queue
    msgsnd(msg_queue_id, msg_to_manager, sizeof(elevator_to_manager), 0);

    // Check if there are more people on the floor that can be picked up
    manager_to_elevator *msg_from_manager = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    while (peopleInElevator(elevator) < MAX_PER_ELEVATOR && msgrcv(floor_queue_id, msg_from_manager, sizeof(manager_to_elevator), floor + 1, IPC_NOWAIT) >= 0)
    {
        if (peoplePerFloor[floor] < 1)
        {
            printf("pickupPeople error: there are no people on that floor\n");
            exit(EXIT_FAILURE);
        }
        peoplePerFloor[floor]--;
        msgsnd(msg_queue_id, msg_to_manager, sizeof(elevator_to_manager), 0);
    }

    free(msg_to_manager);
    free(msg_from_manager);
}

void error_exit(char *error_message)
{
    // fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    perror(error_message);
    exit(1);
}

double clockToMillis(clock_t timeBegin, clock_t timeEnd)
{
    return 1000 * ((double)timeEnd - timeBegin) / CLOCKS_PER_SEC;
}

int getCurrentTicks()
{
    return (clock() - start_t);
}

int clearMsgQueues()
{
    void *msg;

    while (msgrcv(msg_queue_id, NULL, 1024, 0, IPC_NOWAIT) >= 0)
        printf("snens\n");

    msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    while (msgrcv(floor_queue_id, msg, sizeof(manager_to_elevator), 0, IPC_NOWAIT) >= 0)

    free(msg);
    return EXIT_SUCCESS;
}