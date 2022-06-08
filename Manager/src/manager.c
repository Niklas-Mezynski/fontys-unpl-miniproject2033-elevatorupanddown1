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
#include "LL.h"

clock_t start_t;
elevator *elevators[NO_ELEVATORS];
int elevatorsRunning = 0;
pthread_t elevator_threads[NO_ELEVATORS];
pthread_t manager_thread, logger_thread;
int floor_queue_id, msg_queue_id;
int peoplePerFloor[NO_FLOORS] = {0};
bool simulationIsRunning;

// Mutex for thread safe logging and global resource changes
pthread_mutex_t mutex;

int clearMsgQueues();

void startManager()
{
    simulationIsRunning = true;
    // Initialize a message queue to store target floors for the elevators
    initMsgQueue(&floor_queue_id, FLOOR_QUEUE_ID, IPC_CREAT | 0600);
    // Initialize a message queue to store target floors for the elevators
    initMsgQueue(&msg_queue_id, MSG_QUEUE_ID, IPC_CREAT | 0600);
    clearMsgQueues();

    pthread_mutex_init(&mutex, 0);
    // Init the general elevator variables
    initElevatorsGeneral(&mutex);

    // Safe the program start
    start_t = clock();

    // Start the manager thread
    pthread_create(&manager_thread, NULL, managerLoop, NULL);

    // Start the elevators (and it's threads)
    initialzeElevators();

    // Start the manager thread
    pthread_create(&logger_thread, NULL, loggerThread, NULL);

    // Wait for the elevator threads to finish
    for (int i = 0; i < NO_ELEVATORS; i++)
    {
        pthread_join(elevator_threads[i], NULL);
    }
    // Wait for the manager thread
    pthread_join(manager_thread, NULL);
    pthread_join(logger_thread, NULL);

    // Print remainng No of people on each floor
    for (int j = 0; j < NO_FLOORS; j++)
    {
        printf("Remaining ppl at floor %d: %d\n", j, peoplePerFloor[j]);
    }

    pthread_mutex_destroy(&mutex);
}

void *managerLoop()
{
    // /*
    // For the client connection
    int client_socket;
    ssize_t recv_size;
    // For mesages recieved from the client socket sent by the floors
    client_to_manager *rec_msg_floor = (client_to_manager *)malloc(sizeof(client_to_manager));
    // For mesage sending to the client socket
    manager_to_client *send_msg_floor = (manager_to_client *)malloc(sizeof(manager_to_client));
    // For mesages recieved from the message queue sent by the elevator
    elevator_to_manager *rec_msg_elevator = (elevator_to_manager *)malloc(sizeof(elevator_to_manager));
    // For mesages that need to be send to the elevator over the msg_queue
    manager_to_elevator *send_msg_elevator = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));

    // Start the server and recieve a client connection
    initServer(&client_socket);

    // Manager loop -> loops until the day is over and constantly handles the message traffic between floors and elevators
    while (simulationIsRunning)
    {
        // Recieve data from the floor socket
        if ((recv_size = recv(client_socket, rec_msg_floor, sizeof(client_to_manager), 0)) < 0)
            error_exit("Message recieve error");
        if (recv_size == 0)
        {
            printf("Client disconnected... shutting down program\n");
            {
                simulationIsRunning = false;
            }
        }
        // Send the recieved data to the elevators
        if (recv_size > 0)
        {
            printf("Client recieved message - spawn event on floor %d\n", rec_msg_floor->floorID);
            // Put the new target in the msg_queue so the elevators know they have to pick up people from there
            send_msg_elevator->mtype = (rec_msg_floor->floorID + 2);
            send_msg_elevator->destFloor = 0;
            pthread_mutex_lock(&mutex);
            peoplePerFloor[rec_msg_floor->floorID]++;
            pthread_mutex_unlock(&mutex);
            if (msgsnd(floor_queue_id, send_msg_elevator, sizeof(manager_to_elevator) - sizeof(long), IPC_NOWAIT) < 0)
            {
                error_exit("Manager: Cannot send message to elevator msg_queue");
            }
        }

        // Check for incoming messages from the elevators
        // if (checkIncomingMsgs(rec_msg_elevator))
        // {
        //     // Do something with the message (send it to the floor)
        //     // TODO
        //     send_msg_floor->floorID = rec_msg_elevator->floor;
        //     send_msg_floor->noPeopleInElevator = 1;
        //     if (send(client_socket, send_msg_floor, sizeof(manager_to_client), 0) < 0)
        //     {
        //         error_exit("Send message to client socket error");
        //     }
        // }
    }

    // Close the connection
    close(client_socket);
    free(rec_msg_floor);
    free(rec_msg_elevator);
    return EXIT_SUCCESS;
    // */

    /*
    // For testing/debugging -> randomly send messages to the elevators

    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    elevator_to_manager *rec_msg_elevator = (elevator_to_manager *)malloc(sizeof(elevator_to_manager));
    usleep(100 * MILLI_TO_MICRO);
    for (size_t i = 0; i < 256; i++)
    {
        usleep(((rand() % 3000 / 1000)) * MILLI_TO_MICRO);
        // Check for incoming messages by the
        if (checkIncomingMsgs(rec_msg_elevator))
        {
            // Do something with the message (send it to the floor)
            // TODO
        }

        // if (rand() % 10 != 0)
        //     continue;
        // Sometimes, randomly spawn a person on a random floor
        long spawnFloor = ((rand()) % (NO_FLOORS - 1)) + 1;
        msg->mtype = spawnFloor + 2;
        msg->destFloor = 0; // For now everybody's destination is floor 0;
        pthread_mutex_lock(&mutex);
        peoplePerFloor[spawnFloor]++;
        pthread_mutex_unlock(&mutex);
        printf("Spawning someone on floor %ld with dest: %d\n", spawnFloor, msg->destFloor);

        // printf("People spawning at floor %ld \t\t\t\t%f\n", msg->mtype - 1, clockToMillis(0, clock()));
        if (msgsnd(floor_queue_id, msg, sizeof(manager_to_elevator) - sizeof(long), IPC_NOWAIT) == -1)
        {
            error_exit("Message queue floor_queue_id full");
            // perror("Message send error");
            // exit(1);
        }
    }
    // for (int i = 0; i < 100; i++)
    // {
    //     pthread_mutex_lock(&mutex);
    //     printf("Remaining ppl: ");
    //     for (int j = 0; j < NO_FLOORS - 1; j++)
    //     {
    //         printf("%d-%d || ", j, peoplePerFloor[j]);
    //     }
    //     printf("%d: %d\n", NO_FLOORS - 1, peoplePerFloor[NO_FLOORS - 1]);

    //     pthread_mutex_unlock(&mutex);

    //     usleep(25 * MILLI_TO_MICRO);
    // }

    sleep(5);
    for (int j = 0; j < NO_FLOORS; j++)
    {
        printf("Remaining ppl at floor %d: %d\n", j, peoplePerFloor[j]);
    }

    free(msg);
    simulationIsRunning = false;
    return EXIT_SUCCESS;
    */
}

bool checkIncomingMsgs(elevator_to_manager *msg_out)
{

    // IPC_NOWAIT returns an error if there are no messages
    if (msgrcv(floor_queue_id, msg_out, sizeof(elevator_to_manager) - sizeof(long), -1, IPC_NOWAIT) == -1)
    {
        return false;
    }
    return true;
}

void initialzeElevators()
{
    // Create and init the elevator structs and store them in the elevators array
    for (int i = 0; i < NO_ELEVATORS; i++)
    {
        elevators[i] = (elevator *)malloc(sizeof(elevator));
        elevators[i]->id = i;
        elevators[i]->height = 9;           // Starting floor is always 0
        elevators[i]->nextTargetFloor = -1; // No target

        // Set all person pointers to NULL
        // for (int j = 0; j < MAX_PER_ELEVATOR; j++)
        // {
        //     elevators[i]->guestsInside[j] = NULL;
        // }
        elevators[i]->guestList = (LinkedList *)malloc(sizeof(LinkedList));
        constructLL(elevators[i]->guestList);
    }

    for (int i = 0; i < NO_ELEVATORS; i++)
    {
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

void pickupPeople(elevator *elevator, long floor)
{
    if (peoplePerFloor[floor] < 1)
    {
        printf("pickupPeople error: there are no people on that floor (%ld)\n", floor);
        exit(EXIT_FAILURE);
    }
    printf("Elevator %d is now picking up people from floor %ld\t%f\n", elevator->id, floor, clockToMillis(0, clock()));

    // Pick up the guy
    pthread_mutex_lock(&mutex);
    peoplePerFloor[floor]--;
    pthread_mutex_unlock(&mutex);
    elevator_to_manager *msg_to_manager = (elevator_to_manager *)malloc(sizeof(elevator_to_manager));
    msg_to_manager->mtype = ELEVATOR_TO_MANAGER_MTYPE;
    msg_to_manager->floor = floor;
    // Send the event to the msg queue
    msgsnd(msg_queue_id, msg_to_manager, sizeof(elevator_to_manager) - sizeof(long), 0);

    // Check if there are more people on the floor that can be picked up
    manager_to_elevator *msg_from_manager = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    // while (peopleInElevator(elevator) < MAX_PER_ELEVATOR && msgrcv(floor_queue_id, msg_from_manager, sizeof(manager_to_elevator) - sizeof(long), floor + 2, IPC_NOWAIT) >= 0)
    // {
    //     if (peoplePerFloor[floor] < 1)
    //     {
    //         printf("pickupPeople error: there are no people on that floor\n");
    //         exit(EXIT_FAILURE);
    //     }
    //     printf("Elevator %d is now picking up people from floor %ld\t%f\n", elevator->id, floor, clockToMillis(0, clock()));

    //     peoplePerFloor[floor]--;
    //     msgsnd(msg_queue_id, msg_to_manager, sizeof(elevator_to_manager) - sizeof(long), 0);
    // }

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
    void *m = malloc(1024);
    while (msgrcv(msg_queue_id, m, 1024, 0, IPC_NOWAIT) >= 0)
        ;

    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    while (msgrcv(floor_queue_id, msg, sizeof(manager_to_elevator) - sizeof(long), 0, IPC_NOWAIT) >= 0)
        ;

    free(m);
    free(msg);
    printf("Queues cleared\n");
    return EXIT_SUCCESS;
}

void *loggerThread()
{
    FILE *file = fopen("test1.txt", "w");
    clock_t elevatorStartTimes[NO_ELEVATORS] = {};
    clock_t totalWaitingTimes[NO_ELEVATORS] = {0};
    int elevatorIdleCounts[NO_ELEVATORS] = {0};
    // Init array with -1
    for (int i = 0; i < NO_ELEVATORS; i++)
        elevatorStartTimes[i] = -1;

    if (file == NULL)
    {
        error_exit("Manager - Logger thread: file pointer == NULL");
    }

    logger_message *msg = (logger_message *)malloc(sizeof(logger_message));
    while (elevatorsRunning > 0)
    {
        // Check for new information to log
        if (msgrcv(msg_queue_id, msg, sizeof(logger_message) - sizeof(long), LOGGER_THREAD_MTYPE, IPC_NOWAIT) >= 0)
        {
            // There is a new info

            switch (msg->info)
            {
            case StartIdle:
                // Elevator starts to idle -> safe time
                elevatorStartTimes[msg->elevator_id] = msg->time;
                break;
            case StopIdle:
                // Ignore first idle time
                if (elevatorIdleCounts[msg->elevator_id] == 0)
                {
                    elevatorIdleCounts[msg->elevator_id]++;
                    totalWaitingTimes[msg->elevator_id] += 50000;
                    continue;
                }

                // Elevator stops idling -> calc complete idle time and add for average
                totalWaitingTimes[msg->elevator_id] += msg->time - elevatorStartTimes[msg->elevator_id];
                elevatorIdleCounts[msg->elevator_id]++;

                // Print event to logfile
                fprintf(file, "Elevator %d just idled for %ld\n", msg->elevator_id, msg->time - elevatorStartTimes[msg->elevator_id]);
                break;
            default:
                break;
            }
        }
    }

    fprintf(file, "\n----TOTAL IDLE TIMES----\n");
    // Write idle times to the log file
    for (int elevator_id = 0; elevator_id < NO_ELEVATORS; elevator_id++)
    {
        double avg = (double)totalWaitingTimes[elevator_id] / elevatorIdleCounts[elevator_id];
        fprintf(file, "Elevator %d total idle time %ld | Number of idles %d | Avg idle time %f\n", elevator_id, totalWaitingTimes[elevator_id], elevatorIdleCounts[elevator_id], avg);
    }

    fprintf(file, "\n----TOTAL IDLE TIMES (devided by 1000)----\n");
    for (int elevator_id = 0; elevator_id < NO_ELEVATORS; elevator_id++)
    {
        double avg = (double)totalWaitingTimes[elevator_id] / elevatorIdleCounts[elevator_id];
        fprintf(file, "Elevator %d total idle time %ld | Number of idles %d | Avg idle time %f\n", elevator_id, totalWaitingTimes[elevator_id] / 1000, elevatorIdleCounts[elevator_id], avg / 1000);
    }

    fclose(file);
}

void changeElevatorsRunningValue(int val)
{
    elevatorsRunning += val;
}
bool isSimulationRunning()
{
    return simulationIsRunning;
}