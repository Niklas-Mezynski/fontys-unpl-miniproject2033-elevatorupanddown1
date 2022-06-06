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

#define NO_FLOORS 3
#define NO_APARTMENTS 3

#define PORT 8080
#define SA struct sockaddr

int sockfd;
int queue_id;

floorStruct *floors[NO_FLOORS];
pthread_t threads[NO_FLOORS];

// person *persons[NO_APARTMENTS];

pthread_t clientThread;

void startFloors()
{
    initializeSocket();
    // queue_id = msgget(QUEUE_ID, IPC_CREAT | 0600);
    // pthread_create(&clientThread, NULL, floorClient, NULL);
    // initializeFloors();
    // pthread_join(clientThread, NULL);
    // pthread_join(clientThread, NULL);
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
            floors[i]->floorID = i + 1;
            pthread_create(&threads[i], NULL, start, floors[i]);
        }
        else
        {
            floors[i] = malloc(sizeof(floorStruct));
            floors[i]->no_apartments = NO_APARTMENTS;
            floors[i]->floorID = i + 1;
            pthread_create(&threads[i], NULL, start, floors[i]);
        }
    }
    for (i = 0; i < NO_FLOORS; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

void initializeSocket()
{
    int connfd, client_fd;
    struct sockaddr_in servaddr, cli;

    client_to_manager *msgToManager = (client_to_manager *)malloc(sizeof(client_to_manager));

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

    for (size_t i = 0; i < 5; i++)
    {
        msgToManager->floorID = rand() % 10;
        msgToManager->noPeople = rand() % 420;
        send(sockfd, msgToManager, sizeof(client_to_manager), 0);
        sleep(2);
    }

    printf("messages send\n");
    free(msgToManager);
    close(client_fd);
}

void *floorClient()
{
    // TODO recieve and send
    client_to_floor *msg = (client_to_floor *)malloc(sizeof(client_to_floor));
    int rc;
    while (1)
    {
        rc = msgrcv(queue_id, msg, sizeof(client_to_floor), 1, 0);
        if (rc == -1)
        {
            perror("error recieving");
        }
        else
        {
            printf("Msg recieved: Floor ID: %d\n", msg->floorID);
        }
    }
}

void *start(void *floorArguments)
{
    floorStruct *floor = (floorStruct *)floorArguments;
    floor_to_client *msg = (floor_to_client *)malloc(sizeof(floor_to_client));

    // LinkedList *peopleInQueue = (LinkedList* )malloc(sizeof(LinkedList));
    // constructLL(peopleInQueue);

    while (1)
    {
        // sleep til Inter Arrival Time is over
        usleep(nexp());
        // sleep(1);
        // add a person to the mailbox so the floor client can send it to the manager

        msg->mtype = 1;
        msg->floorID = floor->floorID;
        // msg->floorID = 1;
        msg->noPeople = 1;

        if (msgsnd(queue_id, msg, sizeof(floor_to_client), 0) == -1)
        {
            perror("error: ");
        }
    }
    free(msg);
}
