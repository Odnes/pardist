#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 

#define NUMBER_OF_PHILOSOTHREADS     5  //using define instead of global const because of "variably modifiable at file scope" error.

enum states{eating, thinking, hungry};
enum states state_of_philosopher[NUMBER_OF_PHILOSOTHREADS];

pthread_mutex_t action_lock;

void* philosofunc(void* s);

int main(){
pthread_mutex_init(&action_lock, NULL);

pthread_t philosothread[NUMBER_OF_PHILOSOTHREADS];
for (int thread_num = 0; thread_num < NUMBER_OF_PHILOSOTHREADS; thread_num++) {
    printf("Philosopher %d assumes his seat and begins thinking.\n", thread_num);       
    pthread_create(&philosothread[thread_num], NULL, philosofunc, (void*)&thread_num);
    }
 
for (int thread_num = 0; thread_num < NUMBER_OF_PHILOSOTHREADS; thread_num++){
    pthread_join(philosothread[thread_num], NULL);
}

pthread_mutex_destroy(&action_lock);
}
 
void* philosofunc(void* s){
    pthread_mutex_lock(&action_lock);
    int* seat = s;
//   while(1){
        printf("Philosopher in seat %d resumes thinking.\n", *seat);
    pthread_mutex_unlock(&action_lock);
  //   }
  return NULL;
} 

 
 


 //missing stuff: LEFT, RIGHT,  sem_t S[N], seat[N]