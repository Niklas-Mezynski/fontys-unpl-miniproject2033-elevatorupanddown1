#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <stdbool.h>
#include "manager.h"
#include "elevator.h"
#include "LL.h"

int rec_queue_id;
int send_queue_id;

void *start(void *thisElevatorArg)
{
    // Get the elevator from the arg
    elevator *thisElevator = (elevator *)thisElevatorArg;

    // Initialize the mailboxes
    // initMsgQueue(&send_queue_id, QUEUE_ID_ELEVATOR_TO_MANAGER, IPC_CREAT | 0600);
    initMsgQueue(&rec_queue_id, QUEUE_ID_MANAGER_TO_ELEVATOR, 0);

    // Queue for storing the floors the elevator needs to reach
    LinkedList *targetFloorQueue = (LinkedList *)malloc(sizeof(LinkedList));
    constructLL(targetFloorQueue);
    // Mutex to lock queue operations
    pthread_mutex_t queue_lock;
    pthread_mutex_init(&queue_lock, NULL);

    printf("Hello from elevator %d\n", thisElevator->id);

    // Create another thread that is responsible for recieving messages from the manager
    pthread_t msg_thread;
    struct messageThreadArgs *msgThreadArgs = (struct messageThreadArgs *)malloc(sizeof(struct messageThreadArgs));
    msgThreadArgs->thisElevator = thisElevator;
    msgThreadArgs->targetFloorQueue = targetFloorQueue;
    msgThreadArgs->queue_lock = &queue_lock;
    pthread_create(&msg_thread, NULL, recieve_messages, msgThreadArgs);

    // Moving loop for the elevator
    while (1)
    {
        // If elevator has no current target AND there is no target in the queue -> idle
        // if (thisElevator->nextTargetFloor == -1 && isEmpty(targetFloorQueue))
        // {
        //     continue;
        // }

        // // If the elevator currently has no target, but there is something in the queue -> start moving to that floor
        // if (thisElevator->nextTargetFloor == -1 && !isEmpty(targetFloorQueue))
        // {
        //     // Move the elevator towards the floor
        // }
    }
}

void *recieve_messages(void *messageThreadArgs)
{
    // Retrieve the thread args
    elevator *thisElevator = (elevator *)((struct messageThreadArgs *)messageThreadArgs)->thisElevator;
    LinkedList *targetFloorQueue = (LinkedList *)((struct messageThreadArgs *)messageThreadArgs)->targetFloorQueue;
    pthread_mutex_t *queue_lock = (pthread_mutex_t *)((struct messageThreadArgs *)messageThreadArgs)->queue_lock;

    // Struct for incoming messages
    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    while (1)
    {
        // Recieve messages
        if (msgrcv(rec_queue_id, msg, sizeof(manager_to_elevator), (thisElevator->id + 1), 0) < 0)
        {
            perror("Elevator recieve error");
            exit(1);
        }
        printf("Elevator %d recieved message to go to floor %d   %f\n", thisElevator->id, msg->floor, clockToMillis(0, clock()));

        // Add the message to the tail of the floor queue
        pthread_mutex_lock(queue_lock);
        addRearLL(targetFloorQueue, msg->floor);
        pthread_mutex_unlock(queue_lock);
    }
}