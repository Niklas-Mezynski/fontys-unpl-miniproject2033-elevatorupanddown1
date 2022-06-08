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
int calcNextReachableFloor(elevator *elevator);

int floor_queue_id, msg_queue_id;
pthread_mutex_t mutex;

void initElevatorsGeneral(pthread_mutex_t *m)
{
    // Initialize the mailboxe
    initMsgQueue(&floor_queue_id, FLOOR_QUEUE_ID, 0);
    initMsgQueue(&msg_queue_id, MSG_QUEUE_ID, 0);

    mutex = *m;
}

void *start(void *thisElevatorArg)
{
    // Get the elevator from the arg
    elevator *thisElevator = (elevator *)thisElevatorArg;

    // Queue for storing the floors the elevator needs to reach
    // LinkedList *targetFloorQueue = (LinkedList *)malloc(sizeof(LinkedList));
    // constructLL(targetFloorQueue);

    printf("Hello from elevator %d\n", thisElevator->id);
    pthread_mutex_lock(&mutex);
    // Increase by one
    changeElevatorsRunningValue(1);
    pthread_mutex_unlock(&mutex);

    clock_t last_move;
    double targetFloorHeight;
    bool isMooving = true;
    // For recieving messages from manager
    manager_to_elevator *msg_from_manager = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));

    // For sending messages to the logger
    logger_message *msg_logger = (logger_message *)malloc(sizeof(logger_message));
    msg_logger->elevator_id = thisElevator->id;
    msg_logger->mtype = LOGGER_THREAD_MTYPE;

    // Moving loop for the elevator
    while (1)
    {
        // If elevator isn't moving: check for new tasks in the queue
        if (thisElevator->nextTargetFloor == -1)
        {
            // For logging
            if (isMooving)
            {
                isMooving = false;
                // Safe "idle start time"
                msg_logger->info = StartIdle;
                msg_logger->time = clock();
                msgsnd(msg_queue_id, msg_logger, sizeof(logger_message) - sizeof(long), IPC_NOWAIT);
            }

            // TODO
            // if (peopleInElevator(thisElevator) > 0)
            // {
            //     thisElevator->nextTargetFloor =
            // }

            last_move = clock();
            // Check for messages in the queue
            if (msgrcv(floor_queue_id, msg_from_manager, sizeof(manager_to_elevator) - sizeof(long), 0, IPC_NOWAIT) >= 0)
            {
                // Set the target floor
                thisElevator->nextTargetFloor = msg_from_manager->mtype - 2;
                // printf("Elevator %d target floor: %ld\n", thisElevator->id, thisElevator->nextTargetFloor);
                targetFloorHeight = thisElevator->nextTargetFloor * FLOOR_HEIGHT;
            }
            else
            {
                // No current target and no new messages
                if (!isSimulationRunning())
                {
                    pthread_mutex_lock(&mutex);
                    // Decrease by one and end loop
                    changeElevatorsRunningValue(-1);
                    pthread_mutex_unlock(&mutex);
                    break;
                }
                continue;
            }
        }

        // For logging
        if (!isMooving)
        {
            isMooving = true;
            // Safe "idle end time" and send it to the logthread
            msg_logger->info = StopIdle;
            msg_logger->time = clock();
            msgsnd(msg_queue_id, msg_logger, sizeof(logger_message) - sizeof(long), 0);
        }

        // Check if the elevator is at it's target floor (+- 0.1 meter)
        if (thisElevator->nextTargetFloor != -1 && thisElevator->height > targetFloorHeight - 0.1 && thisElevator->height < targetFloorHeight + 0.1)
        {
            // Take all people from that floor and message the manager
            // pthread_mutex_lock(&mutex);
            pickupPeople(thisElevator, thisElevator->nextTargetFloor);
            // pthread_mutex_unlock(&mutex);

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
            // pthread_mutex_lock(&mutex);
            pickupPeople(thisElevator, interruptFloor);
            // pthread_mutex_unlock(&mutex);

            // Sleep for 3 "seconds" (time people need to quit the elevator)
            usleep(3 * MILLI_TO_MICRO);
            last_move = clock();
        }

        // Move the elevator to
        moveElevator(thisElevator, &last_move);
        // printf("Elevator height: %f\n", thisElevator->height);
    }
    free(msg_from_manager);
    free(msg_logger);
}

void moveElevator(elevator *thisElevator, clock_t *last_move)
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
    int nextFloor = calcNextReachableFloor(elevator);
    if (nextFloor == -1 || nextFloor == elevator->nextTargetFloor)
    {
        return result;
    }
    manager_to_elevator *msg_from_manager = (manager_to_elevator *)malloc(sizeof(manager_to_elevator));
    if (msgrcv(floor_queue_id, msg_from_manager, sizeof(manager_to_elevator) - sizeof(long), nextFloor + 2, IPC_NOWAIT) >= 0)
    {
        result = nextFloor;
    }

    free(msg_from_manager);
    // printf("Elevator height: %f, Next floor: %d\n", elevator->height, nextFloor);
    return result;
}

int peopleInElevator(elevator *elevator)
{
    return numbInLL(elevator->guestList);
}

int calcNextReachableFloor(elevator *elevator)
{
    if (elevator->nextTargetFloor == -1)
        return -1;

    movement movement = elevator->nextTargetFloor * FLOOR_HEIGHT > elevator->height ? UP : DOWN;
    if (remainder(elevator->height + (movement * 3), FLOOR_HEIGHT) > -0.1 && remainder(elevator->height, FLOOR_HEIGHT) < 0.1)
    {
        return abs((int)round((elevator->height + (movement * 3)) / FLOOR_HEIGHT));
    }
    return -1;
}