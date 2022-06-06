#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <math.h>
#include "manager.h"
#include "elevator.h"
#include "LL.h"

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

int checkForInterruptionFloor(elevator *elevator);
int calcNextFloor(elevator *elevator);

int floor_queue_id, msg_queue_id;
pthread_mutex_t mutex;

void initElevatorsGeneral()
{
    // Initialize the mailboxe
    initMsgQueue(&floor_queue_id, FLOOR_QUEUE_ID, 0);
    initMsgQueue(&msg_queue_id, MSG_QUEUE_ID, 0);

    pthread_mutex_init(&mutex, 0);
}

void *start(void *thisElevatorArg)
{
    // Get the elevator from the arg
    elevator *thisElevator = (elevator *)thisElevatorArg;

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
    movement movingDirection;
    manager_to_elevator *msg_from_manager = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    // Moving loop for the elevator
    while (1)
    {
        // If elevator isn't moving: check for new tasks in the queue
        if (thisElevator->nextTargetFloor == -1)
        {
            movingDirection = IDLE;
            last_move = clock();
            if (msgrcv(floor_queue_id, msg_from_manager, sizeof(manager_to_elevator), 0, IPC_NOWAIT) < 0)
            {
                // No current target and no new messages
                continue;
            }
            // Set the target floor
            thisElevator->nextTargetFloor = msg_from_manager->mtype - 1;
            targetFloorHeight = thisElevator->nextTargetFloor * FLOOR_HEIGHT;
        }

        // Check if the elevator is at it's target floor (+- 0.1 meter)
        if (thisElevator->nextTargetFloor != -1 && thisElevator->height > targetFloorHeight - 0.1 && thisElevator->height < targetFloorHeight + 0.1)
        {
            // TODO take all people from that floor and message the manager
            pthread_mutex_lock(&mutex);
            pickupPeople(thisElevator, thisElevator->nextTargetFloor);
            pthread_mutex_unlock(&mutex);

            // Update the values of the elevator
            thisElevator->height = targetFloorHeight;
            thisElevator->nextTargetFloor = -1;

            // Sleep for 3 "seconds" (time people need to quit the elevator)
            usleep(3 * MILLI_TO_MICRO);
            last_move = clock();
            continue;
        }

        int interruptFloor = checkForInterruptionFloor(thisElevator);
        if (interruptFloor != -1)
        {
            // Deacceleration time
            usleep(3 * MILLI_TO_MICRO);
            thisElevator->height = interruptFloor * FLOOR_HEIGHT;
            // Pickup the people
            pthread_mutex_lock(&mutex);
            pickupPeople(thisElevator, interruptFloor);
            pthread_mutex_unlock(&mutex);

            // Sleep for 3 "seconds" (time people need to quit the elevator)
            usleep(3 * MILLI_TO_MICRO);
            last_move = clock();
        }

        // Move the elevator to
        moveElevatorAlt(thisElevator, &last_move);

        /*
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
            targetFloorHeight = thisElevator->nextTargetFloor * FLOOR_HEIGHT;
            deleteLL(targetFloorQueue);
            printf("Elevator %d will now move to floor %d\t\t\t%f\n", thisElevator->id, thisElevator->nextTargetFloor, clockToMillis(0, clock()));
            last_move = clock();
        }

        // Check if the elevator is at the queue's first target floor (+- 0.1 meter)
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
        */
    }
}

void recieve_messages(elevator *thisElevator, LinkedList *targetFloorQueue)
{

    // Struct for incoming messages
    manager_to_elevator *msg = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));

    // Check for new messages
    while (msgrcv(floor_queue_id, msg, sizeof(manager_to_elevator), (thisElevator->id + 1), IPC_NOWAIT) >= 0)
    {
        // While there are messages, add them to the target floor queue
        addRearLL(targetFloorQueue, msg->mtype);
    }
    // No (more) new messages --> return
    free(msg);
}

void moveElevatorAlt(elevator *thisElevator, clock_t *last_move)
{
    // Calculate distance to floor 0
    double targetFloorHeight = thisElevator->nextTargetFloor * FLOOR_HEIGHT;
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

/*
    Check if the elevator needs to stop on the next floor because there are people waiting
    (Deacceleration way = 3m)
 */
int checkForInterruptionFloor(elevator *elevator)
{
    int nextReachableFloor;
    int result = -1;
    int nextFloor = calcNextFloor(elevator);
    manager_to_elevator *msg_from_manager = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    if (msgrcv(floor_queue_id, msg_from_manager, sizeof(manager_to_elevator), nextFloor + 1, IPC_NOWAIT) >= 0)
    {
        result = nextFloor;
    }
    
    free(msg_from_manager);
    return result;
}

int peopleInElevator(elevator *elevator)
{
    int amount = 0;
    for (int i = 0; i < MAX_PER_ELEVATOR; i++)
    {
        if (elevator->guestsInside[i] != NULL)
            amount++;
    }
    return amount;
}

int calcNextFloor(elevator *elevator)
{
    if (elevator->nextTargetFloor == -1)
        return -1;

    movement movement = elevator->nextTargetFloor * FLOOR_HEIGHT > elevator->height ? UP : DOWN;
    if (remainder(elevator->height + (movement * 3), FLOOR_HEIGHT) > -0.1 && remainder(elevator->height, FLOOR_HEIGHT) < 0.1)
    {
        return abs((int)round((elevator->height + (movement * 3)) / FLOOR_HEIGHT));
    }
    // if (movement == UP)
    // {

    // }
    // else
    // {
    // }
}