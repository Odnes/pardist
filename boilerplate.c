#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

const int number_of_threads = 4;
sem_t lock;

int main(){
    
    int r = sem_init(&lock, 0, 1);   //works like pthread_create
    if (r<0){                       //just a test it worked
        perror("sem_init failed");
        exit(-1);
    }

    
    pthread_t thread_handles[number_of_threads];
        for (int thread = 0; thread < T; thread++) {       // "thread" is used for iteration and as argument for *rank whose existence seems obligatory
            pthread_create(&thread_handles[thread], NULL, exhibit_A, (void *)thread); //  cast probably redundant
        }

        for (thread = 0; thread < T; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }

    
    
    sem_destroy(&lock); //no more playing with semaphores
}

void exhibit_A(void *rank){
    sem_wait(&lock); //order a binary semaphore to wait(lock)
    sem_post(&lock); //signal (unlock) a binary semaphore

}