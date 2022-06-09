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
#include <sys/msg.h>
#include <sys/ipc.h>
#include "math.h"
#include "floor.h"
#include "random.h"
#include "stdbool.h"
// #include "LL_Floor.h"

#define NO_FLOORS 10
#define NO_APARTMENTS 10

#define PORT 8080
#define SA struct sockaddr

int sockfd, client_fd;
int queue_id, queue_id_2;

bool simulationRunning = true;

int counterFloor;

time_t record_time;

floorStruct *floors[NO_FLOORS];
pthread_t threads[NO_FLOORS];

// person *persons[NO_APARTMENTS];

pthread_t clientThread;
pthread_t serverThread, loggerThread;

pthread_mutex_t mutex;
// LinkedList *floorRecord = (LinkedList*)malloc(sizeof(LinkedList));

void startFloors()
{
    initializeSocket();
    pthread_mutex_init(&mutex, NULL);
    queue_id = msgget(QUEUE_ID, IPC_CREAT | 0600);
    queue_id_2 = msgget(QUEUE_ID_2, IPC_CREAT | 0600);
    //  printf("problem");
    pthread_create(&serverThread, NULL, floorServer, NULL);
    pthread_create(&clientThread, NULL, floorClient, NULL);
    pthread_create(&loggerThread, NULL, loggerThreadToFile, NULL);
    // printf("test");
    initializeFloors();
    pthread_join(serverThread, NULL);
    // pthread_create(&clientThread, NULL, floorClient, NULL);
    pthread_join(loggerThread, NULL);
    pthread_join(clientThread, NULL);
}

void initializeFloors()
{
    int i;
    for (i = 0; i < NO_FLOORS; i++)
    {

        if (i == 0)
        {
            floors[i] = malloc(sizeof(floorStruct));
            floors[i]->no_apartments = 0;
            floors[i]->floorID = i;
            pthread_create(&threads[i], NULL, start, floors[i]);
        }
        else
        {
            floors[i] = malloc(sizeof(floorStruct));
            floors[i]->no_apartments = NO_APARTMENTS;
            floors[i]->floorID = i;
            pthread_create(&threads[i], NULL, start, floors[i]);
        }
        // printf("floor created\n");
    }
    for (i = 0; i < NO_FLOORS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("floors beendet");
}

void initializeSocket()
{
    int connfd;
    struct sockaddr_in servaddr, cli;

    // client_to_manager *msgToManager = (client_to_manager *)malloc(sizeof(client_to_manager));

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (client_fd = connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // for (size_t i = 0; i < 5; i++)
    // {
    // msgToManager->floorID = rand() % 10;
    // msgToManager->noPeople = rand() % 420;
    // send(sockfd, msgToManager, sizeof(client_to_manager), 0);
    // sleep(2);
    // }
}

void *floorClient()
{
    manager_to_client *msgFromManager = (manager_to_client *)malloc(sizeof(manager_to_client));
    msgFromManagerToFloor *msgToFloor = (msgFromManagerToFloor *)malloc(sizeof(msgFromManagerToFloor));

    int rc;
    while (simulationRunning)
    {
        if ((rc = recv(sockfd, msgFromManager, sizeof(msgFromManager), 0)) < 0)
            perror("Message recieve error");

        if (rc > 0)
        {
            time(&record_time);
            printf("Message from server: %d\t%d \t%s\n", msgFromManager->floorID, msgFromManager->noPeopleInElevator, ctime(&record_time));

            msgToFloor->mtype = msgFromManager->floorID + 2;
            // to which Floor the message should go
            // how many people the elevator will pick up
            msgToFloor->noPeople = msgFromManager->noPeopleInElevator;
            if (msgsnd(queue_id_2, msgToFloor, sizeof(msgFromManagerToFloor), 0) == -1)
            {
                perror("error sending information from floor client to wanted floor");
            }
        }
    }
    // printf("floorclient beendet\n");
    free(msgFromManager);
    free(msgToFloor);
}

void *floorServer()
{
    int i = 0;
    int counter = 0;
    
    // TODO recieve and send
    client_to_manager *msgToManager = (client_to_manager *)malloc(sizeof(client_to_manager));

    client_to_floor *msg = (client_to_floor *)malloc(sizeof(client_to_floor));
    int rc;
    while (simulationRunning)
    {
        // printf("error");
        pthread_mutex_lock(&mutex);
        rc = msgrcv(queue_id, msg, sizeof(client_to_floor), 1, IPC_NOWAIT);
        if (rc >= 0)
        {
            // handle msg
            msgToManager->floorID = msg->floorID;
            msgToManager->noPeople = 1;
            time(&record_time);
            send(sockfd, msgToManager, sizeof(client_to_manager), 0);
            // sleep(2);
            // printf("Send at:\t%s\n", ctime(&record_time));
            printf("Send msg to manager: %d\n", msgToManager->floorID);

            // addFrontLL(floorRecord, i);
            counter++;
        }
        i++;
        pthread_mutex_unlock(&mutex);
    }
    // printLL(floorRecord);
    // printf("messages send\n");
    printf("floor server beendet\n");
    close(client_fd);
    free(msgToManager);
    free(msg);
}

void *start(void *floorArguments)
{
    msgFromManagerToFloor *msgToFloor = (msgFromManagerToFloor *)malloc(sizeof(msgFromManagerToFloor));

    
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    constructLL(list);

    floorStruct *floor = (floorStruct *)floorArguments;
    floor_to_client *msg = (floor_to_client *)malloc(sizeof(floor_to_client));

    // keep an eye on the people on a floor because only a max no. is allowed
    // int counter = 0;
    subThreadStruct *sub = (subThreadStruct *)malloc(sizeof(subThreadStruct));
    sub->floorID = floor->floorID;
    sub->list = list;

    pthread_t floorSubThread;
    pthread_create(&floorSubThread, NULL, floorMessageReceive, sub);

    for (size_t i = 0; i < 100; i++)
    {
        if (true)
        {
            // sleep til Inter Arrival Time is over
            usleep(nexp());
            // usleep(1000 * 500);
            // add a person to the mailbox so the floor client can send it to the manager

            pthread_mutex_lock(&mutex);
            counterFloor++;
            msg->mtype = 1;
            msg->floorID = floor->floorID;
            person *personData = (person *)malloc(sizeof(person));
            personData->spawnTime = clock();
            addRearLL(list, personData);
            if (msgsnd(queue_id, msg, sizeof(floor_to_client), 0) == -1)
            {
                perror("error: ");
            }
        }
        // printLL(list);
        // printf("counter: %d\n", counterFloor);
        pthread_mutex_unlock(&mutex);
        // TODO get msg from manager how many people he picks up, if people are collected calculate difference to get waiting time
        /*
        if (msgrcv(queue_id_2, msgToFloor, sizeof(msgFromManagerToFloor), floor->floorID + 1, IPC_NOWAIT) >= 0)
        {
            counter = counter - msgToFloor->noPeople;
        }
        */
        // ToDo reduce counter by the number of people the elevator picked up
        // counter++;
    }
    simulationRunning = false;
    pthread_join(floorSubThread, NULL);
    free(msg);
    free(msgToFloor);
    free(floor);
}

void *floorMessageReceive(void *args)
{
    

    msgFromManagerToFloor *msgToFloor = (msgFromManagerToFloor *)malloc(sizeof(msgFromManagerToFloor));
    subThreadStruct *floor = (subThreadStruct *)args;
    LinkedList *list = floor->list;
    person *personData = (person *)malloc(sizeof(person));

    infoToLogger *logger = (infoToLogger *)malloc(sizeof(infoToLogger));

    clock_t diff;
    while (simulationRunning)
    {

        if (msgrcv(queue_id_2, msgToFloor, sizeof(msgFromManagerToFloor), floor->floorID + 2, IPC_NOWAIT) >= 0)
        {
            // do something
            // info->noPeople = msgToFloor->noPeople;
            printf("msg from manager arrived: %d , %d\n", floor->floorID, msgToFloor->noPeople);
            if(numbInLL(list) < msgToFloor->noPeople)  {
                printf("error: people in list wrong\n");
                exit(EXIT_FAILURE);
            }
            for (size_t i = 0; i < msgToFloor->noPeople; i++)
            {
                diff = clock() - list->head->value->spawnTime;
                // printf("Time difference: %ld\n", diff);
                // fprintf(file, "FloorID: %d\tTime difference: %ld\n", floor->floorID, diff/1000);
                logger->mtype = 3;
                logger->floorID = floor->floorID;
                logger->timeDidd = diff;
                msgsnd(queue_id, logger, sizeof(infoToLogger), 0);
                deleteLL(list);
            }
            
                
        }
    }
    printf("floor msg rec beendet\n");
    
    free(msgToFloor);
    free(personData);
    // free(logger);
}

void* loggerThreadToFile() 
{
    infoToLogger *logger = (infoToLogger*)malloc(sizeof(infoToLogger));
    remove("data.txt");
    FILE *file;
    file = fopen("data.txt", "w");
    int counter;
    clock_t sum;
    if (file == NULL)
    {
        printf("error");
    }
    while(simulationRunning)
    {
        if(msgrcv(queue_id, logger, sizeof(infoToLogger), 3, IPC_NOWAIT) >= 0)
        {
            
            sum += logger->timeDidd;
            // printf("FloorId: %d\tTime difference: %ld\n", logger->floorID, logger->timeDidd);
            fprintf(file, "FloorID: %d\tTime difference: %ld\n", logger->floorID, logger->timeDidd/1000);
            counter++;
        }
    }
    fprintf(file, "Total avg waiting time: %f\n", (double)(sum/counter));
    fclose(file);
    free(logger);
}

double clockToMillis(clock_t timeBegin, clock_t timeEnd)
{
    return 1000 * ((double)timeEnd - timeBegin) / CLOCKS_PER_SEC;
}