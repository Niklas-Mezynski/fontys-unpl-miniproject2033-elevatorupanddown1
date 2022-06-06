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

int queue_id;
/*
       1000 ticks -> 1 sec
      60000 ticks -> 1 min
    3600000 ticks -> 1 hour
   86400000 ticks -> 24 hours
*/

/*  THIS ONE IS CURRENTLY IN USE
       1ms -> 1 sec
      60ms -> 1 min
    3600ms -> 1 hour
   86400ms -> 24 hours
*/

void *start(void *thisElevatorArg)
{
    // Get the elevator from the arg
    elevator *thisElevator = (elevator *)thisElevatorArg;

    // Initialize the mailboxe
    initMsgQueue(&queue_id, QUEUE_ID, 0);

    // Queue for storing the floors the elevator needs to reach
    LinkedList *targetFloorQueue = (LinkedList *)malloc(sizeof(LinkedList));
    constructLL(targetFloorQueue);

    printf("Hello from elevator %d\n", thisElevator->id);

    // Create another thread that is responsible for recieving messages from the manager
    // pthread_t msg_thread;
    // struct messageThreadArgs *msgThreadArgs = (struct messageThreadArgs *)malloc(sizeof(struct messageThreadArgs));
    // msgThreadArgs->thisElevator = thisElevator;
    // msgThreadArgs->targetFloorQueue = targetFloorQueue;
    // msgThreadArgs->queue_lock = &queue_lock;
    // pthread_create(&msg_thread, NULL, recieve_messages, msgThreadArgs);

    clock_t last_move;
    double targetFloorHeight;
    // Moving loop for the elevator
    while (1)
    {
        // Check for new tasks
        recieve_messages(thisElevator, targetFloorQueue);

        // If elevator has no current target AND there is no target in the queue -> idle
        if (thisElevator->nextTargetFloor == -1 && isEmpty(targetFloorQueue))
        {
            // printf("Elevator %d idling.\n", thisElevator->id);
            usleep(1 * MILLI_TO_MICRO);
            last_move = clock();
            continue;
        }

        // If there is something in the queue and the elevator currently has no target -> schedule the target
        if (thisElevator->nextTargetFloor == -1 && !isEmpty(targetFloorQueue))
        {
            thisElevator->nextTargetFloor = targetFloorQueue->head->value;
            targetFloorHeight = thisElevator->nextTargetFloor * 4.5;
            deleteLL(targetFloorQueue);
            printf("Elevator %d will now move to floor %d\t\t\t%f\n", thisElevator->id, thisElevator->nextTargetFloor, clockToMillis(0, clock()));
            last_move = clock();
        }

        // Check if the elevator is at the queue's first target floor (+- 0.1 meter)
        // if (thisElevator->nextTargetFloor != -1 && targetFloorHeight - 0.1 < thisElevator->height < targetFloorHeight * 4.5 + 0.1)
        if (thisElevator->nextTargetFloor != -1 && thisElevator->height > targetFloorHeight - 0.1 && thisElevator->height < targetFloorHeight + 0.1)
        {
            printf("Elevator %d is now picking up people from floor %d\t%f\n", thisElevator->id, thisElevator->nextTargetFloor, clockToMillis(0, clock()));
            // Update the values of the elevator
            thisElevator->height = targetFloorHeight;
            thisElevator->nextTargetFloor = -1;
            // TODO take all people from that floor and message the manager

            // Sleep for 3 "seconds" (time people need to quit the elevator)
            usleep(3 * MILLI_TO_MICRO);
            last_move = clock();
            continue;
        }

        // If the elevator currently has a target -> move towards it
        if (thisElevator->nextTargetFloor != -1)
        {
            // Move the elevator towards target floor
            moveElevatorAlt(thisElevator, &last_move);
            // printf("Elevator %d is moving...\n", thisElevator->id);
        }
    }
}

void recieve_messages(elevator *thisElevator, LinkedList *targetFloorQueue)
{

    // Struct for incoming messages
    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));

    // Check for new messages
    while (msgrcv(queue_id, msg, sizeof(manager_to_elevator), (thisElevator->id + 1), IPC_NOWAIT) >= 0)
    {
        // While there are messages, add them to the target floor queue
        addRearLL(targetFloorQueue, msg->floor);
    }
    // No (more) new messages --> return
    free(msg);
}

// void *moveElevator(void *thisElevatorArg)
// {
//     elevator *thisElevator = (elevator *)thisElevatorArg;
//     // Calculate distance to floor 1
//     double targetFloorHeight = thisElevator->nextTargetFloor * 4.5;
//     clock_t start_t;

//     // TODO: Implement stop process of the elevator for stopping at exact floor height
//     if (thisElevator->height < targetFloorHeight)
//     {
//         start_t = clock();
//         while (!(thisElevator->height >= targetFloorHeight))
//         {
//             usleep(1);
//             if ((clock() - start_t) < 2000)
//             {
//                 thisElevator->height += 0.001;
//                 continue;
//             }
//             if ((clock() - start_t) < 3000)
//             {
//                 thisElevator->height += 0.002;
//                 continue;
//             }
//             thisElevator->height += 0.003;
//         }
//     }
//     else if (thisElevator->height > targetFloorHeight)
//     {
//         start_t = clock();
//         while (!(thisElevator->height <= targetFloorHeight))
//         {
//             usleep(1);
//             if ((clock() - start_t) < 2000)
//             {
//                 thisElevator->height -= 0.001;
//                 continue;
//             }
//             if ((clock() - start_t) < 3000)
//             {
//                 thisElevator->height -= 0.002;
//                 continue;
//             }
//             thisElevator->height -= 0.003;
//         }
//     }
//     thisElevator->nextTargetFloor = -1;
// }

void moveElevatorAlt(elevator *thisElevator, clock_t *last_move)
{
    // Calculate distance to floor 0
    double targetFloorHeight = thisElevator->nextTargetFloor * 4.5;
    double move_diff = clockToMillis(*last_move, clock());
    *last_move = clock();
    if (thisElevator->height < targetFloorHeight)
    {
        thisElevator->height += ELEVATOR_SPEED * move_diff;
    }
    else if (thisElevator->height > targetFloorHeight)
    {
        thisElevator->height -= ELEVATOR_SPEED * move_diff;
    }
}