#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include "floor.h"
#include "random.h"

#define NO_FLOORS 3
#define NO_APARTMENTS 3

#define PORT 8080
#define SA struct sockaddr

int sockfd;

floorStruct *floors[NO_FLOORS];
pthread_t threads[NO_FLOORS];

person *persons[NO_APARTMENTS];

pthread_t clientThread;

void startFloors() {
    initializeFloors();

    // pthread_join(clientThread, NULL);
}

void initializeFloors() 
{
    int i;
    for (i = 0; i < NO_FLOORS; i++) 
    {
        
        if(i == 0) 
        {
            floors[i] -> no_apartments = 0;
            floors[i] -> id = i + 1;
            pthread_create(&threads[i], NULL, start, floors[i]);
        } else {
            floors[i] -> no_apartments = NO_APARTMENTS;
            floors[i] -> id = i + 1;
            pthread_create(&threads[i], NULL, start, floors[i]);
        }
        
    }
    for (i = 0; i < NO_FLOORS; i++) 
    {
        pthread_join(threads[i], NULL);
    }

}


void initializeSocket() {
    int connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
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
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    
}

void* floorClient() {
    //TODO recieve and send
    floor_to_manager *msg = (floor_to_manager *)malloc(sizeof(floor_to_manager));

    while(1)
    {
        if(msgrcv(sockfd, msg, sizeof(floor_to_manager), 0) == -1) {printf("error sending message")};
    }
}

void* start(void* floorArguments) {
    floorStruct* floor = (floorStruct* )floorArguments;
    LinkedList *peopleInQueue = (LinkedList* )malloc(sizeof(LinkedList));
    constructLL(peopleInQueue);
    pthread_create(&clientThread, NULL, floorClient, NULL);

    while(1){
        //sleep til Inter Arrival Time is over
        sleep(nexp());
        //then add one person to list, that want to get collected by the elevator
        addRearLL(peopleInQueue, 1);
    }
    

}
