#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include "manager.h"
#include "elevator.h"

int queue_id;

void *start(void *thisElevatorArg)
{
    queue_id = msgget(QUEUE_ID, 0);
    elevator *thisElevator = (elevator *)thisElevatorArg;
    printf("Hello from elevator %d\n", thisElevator->id);

    messageStruct msg;
    // int messageSize = sizeof(msg) + 1 + MAX_MSG_SIZE;
    // msg = (messageStruct *)malloc(messageSize);

    while (1)
    {
        // sleep(rand() % 4);
        printf("T0\n");
        if (msgrcv(queue_id, &msg, sizeof(messageStruct) + 1, 0, 0) == -1)
        {
            printf("T1\n");
            perror("Elevator recieve error");
            exit(1);
        }
        printf("T2\n");
        printf("Elevator %d recieved message to go to floor %d\n", thisElevator->id, msg.floor);
        // sleep(1);
    }
}