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

#define MICRO_TO_MILLI 1000

clock_t start_t;
elevator *elevators[NO_ELEVATORS];
pthread_t elevator_threads[NO_ELEVATORS];
pthread_t manager_thread;
int queue_id;

void startManager()
{
    // Initialize a message queue for message exchange between elevators and manager
    initMsgQueue(&queue_id);

    // Start the elevators (and it's threads)
    initialzeElevators();

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
    start_t = clock();
}

void *managerLoop()
{
    int client_socket;
    char msg_buffer[BUFFER_SIZE];
    int recv_size;
    time_t rec_time;
    /*
    //Start the server and recieve a client connection
    initServer(&client_socket);

    while (1)
    {
        // Recieve data
        if ((recv_size = recv(client_socket, msg_buffer, BUFFER_SIZE, 0)) < 0)
            error_exit("Message recieve error");

        // Do something with the data (for now, just print it)
        msg_buffer[recv_size] = '\0';
        time(&rec_time);
        printf("Client message: %s \t%s\n", msg_buffer, ctime(&rec_time));
    }

    // Close the connection
    close(client_socket);
    */

    // For independency between manager and floor while coding
    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    while (1)
    {
        usleep(50 * MICRO_TO_MILLI);
        if (rand() % 10 != 0)
            continue;
        // Sometimes, randomly spawn a person on a random floor
        // int messageSize = sizeof(msg) + 1 + MAX_MSG_SIZE;
        // msg = (manager_to_elevator *)malloc(messageSize);
        msg->mtype = (rand() % NO_ELEVATORS) + 1;
        msg->floor = (rand() % NO_FLOORS);
        // printf("Test %d\n", msg->targetFloor);
        if (msgsnd(queue_id, msg, sizeof(manager_to_elevator), 0) == -1)
        {
            error_exit("Message send error");
            // perror("Message send error");
            // exit(1);
        }
    }
    free(msg);
    return EXIT_SUCCESS;
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

    return 1;
}

void initMsgQueue(int *target_queue_id)
{
    // create a public message queue, with access only to the owning user
    *target_queue_id = msgget(QUEUE_ID, IPC_CREAT | 0600);
    if (queue_id == -1)
    {
        perror("Manager: error creating the msg queue with msgget");
        exit(1);
    }
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

int getCurrentTicks() {
    return (clock() - start_t);
}