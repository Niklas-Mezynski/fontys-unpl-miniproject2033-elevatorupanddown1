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
pthread_t elevator_threads[NO_ELEVATORS];
pthread_t manager_thread;

void startManager()
{
    // Start the elevators (and it's threads)
    initialzeElevators();

    // Start the manager thread
    pthread_create(&manager_thread, NULL, managerLoop, NULL);

    // Wait for the elevator threads to finish
    for (int i = 0; i < NO_ELEVATORS; i++)
    {
        pthread_join(elevator_threads[i], NULL);
    }
    // Wait for the manager thread
    pthread_join(manager_thread, NULL);
}

void initialzeElevators()
{
    // Create the elevator structs and store them in the elevators array
    for (int i = 0; i < NO_ELEVATORS; i++)
    {
        elevators[i] = (elevator *)malloc(sizeof(elevator));
        elevators[i]->id = i;
        elevators[i]->height = 0;                                                        // Starting floor is always 0
        elevators[i]->nextTargetFloor = -1;                                              // No target
        elevators[i]->guestsInside = (guest **)malloc(MAX_PER_ELEVATOR * sizeof(guest)); // Allocate memory for the array of guests inside

        // Create the threads
        pthread_create(&elevator_threads[i], NULL, start, elevators[i]);
    }
}

void *managerLoop()
{
}