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
/*
       1000 ticks -> 1 sec
      60000 ticks -> 1 min
    3600000 ticks -> 1 hour
   86400000 ticks -> 24 hours
*/

int queue_id;

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
<<<<<<< HEAD
        // if (thisElevator->nextTargetFloor == -1 && isEmpty(targetFloorQueue))
        // {
        //     continue;
        // }

        // // If the elevator currently has no target, but there is something in the queue -> start moving to that floor
        // if (thisElevator->nextTargetFloor == -1 && !isEmpty(targetFloorQueue))
        // {
        //     // Move the elevator towards the floor
        // }
=======
        if (thisElevator->nextTargetFloor == -1 && isEmpty(targetFloorQueue))
        {
            printf("Elevator %d idling.\n", thisElevator->id);
            usleep(1000);
            continue;
        }

        // If the elevator currently has no target, but there is something in the queue -> start moving to that floor
        if (thisElevator->nextTargetFloor == -1 && !isEmpty(targetFloorQueue))
        {
            // Move the elevator towards the floor
            thisElevator->nextTargetFloor = targetFloorQueue->head->value;
            pthread_create(&thisElevator->movement, NULL, moveElevator, thisElevator);
            // Wait for moving process to finish
            pthread_join(thisElevator->movement, NULL);
            pthread_exit(thisElevator->movement);
            deleteLL(targetFloorQueue);
        }
>>>>>>> 641124ca3d367b4e3ac7b67fb1192bdeea04b18b
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

void *moveElevator(void *thisElevatorArg) {
    elevator *thisElevator = (elevator *)thisElevatorArg;
    // Calculate distance to floor 1
    double targetFloorHeight = thisElevator->nextTargetFloor * 4.5;
    clock_t start_t;

    // TODO: Implement stop process of the elevator for stopping at exact floor height
    if (thisElevator->height < targetFloorHeight)
    {
        start_t = clock();
        while (!(thisElevator->height >= targetFloorHeight))
        {
            usleep(1);
            if ((clock() - start_t) < 2000)
            {
                thisElevator->height += 0.001;
                continue;
            }
            if ((clock() - start_t) < 3000)
            {
                thisElevator->height += 0.002;
                continue;
            }
            thisElevator->height += 0.003;
        }
    } else if (thisElevator->height > targetFloorHeight) {
        start_t = clock();
        while (!(thisElevator->height <= targetFloorHeight))
        {
            usleep(1);
            if ((clock() - start_t) < 2000)
            {
                thisElevator->height -= 0.001;
                continue;
            }
            if ((clock() - start_t) < 3000)
            {
                thisElevator->height -= 0.002;
                continue;
            }
            thisElevator->height -= 0.003;
        }
    }
    thisElevator->nextTargetFloor = -1;
}