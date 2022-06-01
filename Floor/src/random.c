#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
//#include "../../Manager/inc/manager.h"

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int counter = 0;

pthread_t producer_thread, consumer_thread;
pthread_mutex_t mutex;
sem_t  *empty, *full;

double lambda = 1.0;
double mu = 1.0;

void usage() {
    printf("Please use: ./a.out lambda mu\n");
    printf("      with: lambda and mu intergers * 10\n");
    printf("      e.g.: ./a.out 25 13 uses lambda=2.5 and mu=1.3\n");
}

//
// Unlink semaphores /emtpy_sem and /full_sem
//
void unlinkSemaphores() {
    int rv;
    rv = sem_unlink("/empty_sem");     
    if (rv==-1) {
	// if unlinking does not work, /empty_sem does not exist, and continue...
	perror("Failed to unlink /empty_sem\n"); 
	//exit(-1);                              
    }
    sem_unlink("/full_sem");
    if (rv==-1) {
	// if unlinking does not work, /full_sem does not exist, and continue...
	perror("Failed to unlink /full_sem\n");
	//exit(-1);                           
    }
}

//
// Open semaphores /empty_sem and /full_sem"
//
void openSemaphores() {
    empty = sem_open("/empty_sem", O_CREAT | O_EXCL, 0644, BUFFER_SIZE); //initial value = BUFFER_SIZE
    if (empty == SEM_FAILED) {
        perror("Failed to open semphore for empty");
        exit(-1);
    }
    full = sem_open("/full_sem", O_CREAT | O_EXCL, 0644, 0); //initial value = 0
    if (full == SEM_FAILED) {
        perror("Failed to open semphore for full");
        exit(-1);
    } 
}
    

//
// lambda = arrival rate = 1/IAT; IAT = Inter Arrival Time
// newIAT = -log(rand())/lambda
// newIAT is calculated in us (microseconds)
//
int nexp() {
    double newIAT;
    double rnd;

    rnd = (double) (rand()%1000) / 1000; // rnd element of [0, 1]
    newIAT = -1000000*(log(rnd)/lambda); // calculation of newIAT (-log(rnd)/lambda), in microseconds
    return (int)newIAT;
}

void print_buffer(int counter) {
    //for (int i = 0; i < counter; i ++) {
    //    printf("*");
    //}
    printf("%d", counter); // print number of elements in buffer
    printf("\n");
}

void* producer(void* var) {
    int item;
    int rnd;
    int i;

    printf("Producer started\n");
    //if floor has already 6 people its not possible to spawn anymore
    for (i = 0; i< 50; i++) {
        item = rand() % 100 + 1;
	//rnd = 500000 + rand()%500000; //  sleep element of [0.5, 1.0]
	rnd = nexp(); // exponentially distributed;
	usleep(rnd + 1000);

        sem_wait(empty);

        pthread_mutex_lock(&mutex);
        //printf("Producer: empty received\n");

        //Buffer size = max number of persons
        if(counter < BUFFER_SIZE) {
            buffer[counter] = item;
            counter ++;
        }
        print_buffer(counter);

	//sleep(1);

        pthread_mutex_unlock(&mutex);

        //printf("Producer: sending empty\n");
        sem_post(full);
    }
    printf("Producer ready\n");
    return 0;
}


//should only get invoked if elevator
void* consumer(void* var) {
    int item;
    int rnd;
    printf("Consumer started\n");
    while(1) {
	//rnd = 750000 + rand()%50000; //   uniform distributed: sleep element of [0.75, 0.80]
	//rnd = 950000 + rand()%100000; //   uniform distributed: sleep element of [0.95, 1.05]
	rnd = 1000000/mu + (rand()%250000 - 125000); //   uniform distributed: sleep element of [0.95, 1.05]
	usleep(rnd);

        sem_wait(full);

        pthread_mutex_lock(&mutex);
        //printf("Consumer: full received\n");

        if(counter > 0) {
            counter --;
        }
        print_buffer(counter);

        //sleep(1);

        pthread_mutex_unlock(&mutex);
        //printf("Consumer: sending empty\n");
        sem_post(empty);
    }
}

// int initializeRandom() {
//     // int rv;
    
//     // if (argc==3) {
// 	// lambda = (double) atoi(argv[1]) / 10;
// 	// mu = (double) atoi(argv[2]) / 10;
// 	// if ((lambda == 0) || (mu == 0)) {
// 	//     usage();
// 	//     exit(0);
// 	// }
//     // }
//     // else {
// 	// if (argc>1) {
// 	//     usage();
// 	//     exit(0);
// 	// }
//     // }

//     // printf("Lambda = %f\n", lambda);
//     // printf("Mu     = %f\n", mu);
//     // printf("\n");

//     // // initialize randomizer
//     // srand(time(0));                    
     
//     // // initialize mutex
//     // pthread_mutex_init(&mutex, NULL);  
    
//     // // initialize semaphores 
//     // unlinkSemaphores();
//     // openSemaphores();

//     // //
//     // // Create threads for producer and consumer
//     // // 
//     // pthread_create(&producer_thread, NULL, producer, NULL);
//     // // wait 2 seconds before starting consumer
//     // sleep(2);
//     // //pthread_create(&consumer_thread, NULL, consumer, NULL);

//     // pthread_join(producer_thread, NULL);
//     // //pthread_join(consumer_thread, NULL);

//     // sem_close(empty);
//     // sem_close(full);
//     // pthread_exit(NULL);

//     return 0;
// }
