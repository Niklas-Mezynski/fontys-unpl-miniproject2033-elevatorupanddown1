// Server side C/C++ program to demonstrate Socket programming
// See: https://www.geeksforgeeks.org/socket-programming-cc/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "manager.h"
#include "elevator.h"


void *start(void *thisElevatorArg)
{
    elevator *thisElevator = (elevator *)thisElevatorArg;
    while (1)
    {
        sleep(rand() % 4);
        printf("Hello from elevator %d\n", thisElevator->id);
    }
}