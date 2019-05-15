/*
 *  SleepBarber.c
 *
 *   Compile with:
 *	cc sleepBarber.c -o sleepBarber -lpthread -lm
 */

#define _REENTRANT //thread safety compiler flag

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#include <sys/errno.h>

// Function prototypes...
void *customer(void *num);
void *barber(void *);
void randwait(int secs);

// Define the semaphores.

// waitingRoom Limits the # of customers allowed 
// to enter the waiting room at one time.
sem_t waitingRoom;   

// barberChair ensures mutually exclusive access to
// the barber chair.
sem_t barberChair;

// barberPillow is used to allow the barber to sleep
// until a customer arrives.
sem_t barberPillow;

// seatBelt is used to make the customer to wait until
// the barber is done cutting his/her hair. 
sem_t seatBelt;

//guards critical section where customer() pulls
//a unique id from global counter new_id.
pthread_mutex_t id_lock;
int new_id = 1; 

// Flag to stop the barber thread when all customers
// have been serviced.
int allDone = 0;


int main(int argc, char *argv[]) {
    pthread_mutex_init(&id_lock, NULL);

    //Initialize with cmd arguments converted to int.
    int traffic_load_factor = atoi(argv[1]);  //Number of parallel customer() threads.
    int numChairs = atoi(argv[2]);
    long RandSeed = atoi(argv[3]);
    
    pthread_t tid[traffic_load_factor];
    pthread_t btid;
        
    // Check to make sure there are the right number of
    // command line arguments.
    if (argc != 4) {
	printf("Use: SleepBarber <Traffic Load Factor> <Num Chairs> <rand seed>\n");
	exit(-1);
    }
    
    printf("\nSleepBarber.c\n\n");
    printf("A solution to the sleeping barber problem using semaphores.\n");
    
    // Initialize the random number generator with a new seed.
    srand48(RandSeed);
		
    // Initialize the semaphores with initial values...
    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);
    
    // Create the barber.
    pthread_create(&btid, NULL, barber, NULL);

    // Create the customers.
    for(int thread_num = 0; thread_num < traffic_load_factor; thread_num++){
        pthread_create(&tid[thread_num], NULL, customer, NULL);
    }

    // Join each of the threads to wait for them to finish.
    for(int thread_num = 0; thread_num < traffic_load_factor; thread_num++){
    pthread_join(tid[thread_num], NULL);
    } //refactor
    
    pthread_join(btid,NULL);	

    pthread_mutex_destroy(&id_lock);
}

void *customer(void *unused) {
    int id = -1;
    while(1){
        pthread_mutex_lock(&id_lock); //fetch unique id for customer
        id = new_id;
        new_id++;
        pthread_mutex_unlock(&id_lock);    

        // Leave for the shop and take some random amount of
        // time to arrive.
        printf("Customer %d leaving for barber shop.\n", id);
        randwait(5);
        printf("Customer %d arrived at barber shop.\n", id);

        // Wait for space to open up in the waiting room...
        if (sem_trywait(&waitingRoom) == -1) {
        printf("Waiting room full. Customer %d IS LEAVING.\n",id);
        }
    
        else{
        printf("Customer %d entering waiting room.\n", id);

        // Wait for the barber chair to become free.
        sem_wait(&barberChair);
    
        // The chair is free so give up your spot in the
        // waiting room.
        sem_post(&waitingRoom);

        // Wake up the barber...
        printf("Customer %d waking the barber.\n", id);
        sem_post(&barberPillow);

        // Wait for the barber to finish cutting your hair.
        sem_wait(&seatBelt);
    
        // Give up the chair.
        sem_post(&barberChair);
        printf("Customer %d leaving barber shop.\n", id);
        }
    }   
}

void *barber(void *unused) {
    while (1) { //a workaholic!

	// Sleeps until a customer() arrives and wakes him up.
	printf("The barber is sleeping\n");
	sem_wait(&barberPillow);

	// Take a random amount of time to cut the
	// customer's hair.
	printf("The barber is cutting hair\n");
	randwait(3);
	printf("The barber has finished cutting hair.\n");

	// Release the customer when done cutting...
	sem_post(&seatBelt);
    }
}

void randwait(int secs) {
    int len;
	
    // Generate a random number...
    len = (int) ((drand48() * secs) + 1);
    sleep(len);
}