// Server side C/C++ program to demonstrate Socket programming
// See: https://www.geeksforgeeks.org/socket-programming-cc/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "manager.h"
#include "elevator.h"

elevator *elevators[NO_ELEVATORS];
pthread_t threads[NO_ELEVATORS];

void initialzeElevators()
{
    int i;
    // Create the elevator structs and store them in the output array
    for (i = 0; i < NO_ELEVATORS; i++)
    {
        elevators[i] = (elevator *)malloc(sizeof(elevator));
        elevators[i]->id = i;
        elevators[i]->height = 0;                                                        // Starting floor is always 0
        elevators[i]->nextTargetFloor = -1;                                              // No target
        elevators[i]->guestsInside = (guest **)malloc(MAX_PER_ELEVATOR * sizeof(guest)); // Allocate memory for the array of guests inside

        // Create the threads
        pthread_create(&threads[i], NULL, start, elevators[i]);
    }

    for (i = 0; i < NO_ELEVATORS; i++)
    {
        pthread_join(threads[i], NULL);
    }
}