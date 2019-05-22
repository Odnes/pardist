/*
    sleepingbbq.cpp
    by thcloak
    
    Compile with:
    g++ -Wall ./sleepingbbq.cpp -o sleepingbbq -lpthread -std=c++11
*/

#include <pthread.h>
#include <semaphore.h>
#include <thread>
#include <chrono>
#include <iostream>

void *customer(void *);
void *barber(void *);
void randWait(int seconds);

sem_t waitingRoom;
sem_t barberChair;
sem_t barberPillow;
sem_t seatBelt;

//guards critical section where customer() pulls
//a unique id from global counter new_id.
pthread_mutex_t id_lock;
int new_id{1}; 

int main(int argc, char* argv[]){
    pthread_mutex_init(&id_lock, NULL);

    int numChairs = atoi(argv[1]);
    long randSeed = atoi(argv[2]);

    pthread_t btid;
    pthread_t tid[5];

    if(argc!=3){
        std::cout << "Use: ./sleepingbbq <Num Chairs> <rand seed>\n";
        exit(-1);
    }

    std::cout  << "\n sleepingbbq.cpp\n\n";
    std::cout << "A joint solution to the sleeping barber problem and\n the producer\
 consumer problem, using semaphores.\n\n";

    srand(randSeed);

    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);

    pthread_create(&btid, NULL, barber, NULL);
    
    int thread_num{0};
    for (thread_num = 0; thread_num < 5; thread_num++) {
        pthread_create(&tid[thread_num], NULL, customer, NULL);
    }
    for (thread_num=0; thread_num<5; thread_num++) {
	    pthread_join(tid[thread_num],NULL);
    }

    sem_post(&barberPillow);
    pthread_join(btid, NULL);

    pthread_mutex_destroy(&id_lock);
}

    void *customer(void *unused){
        int id{-1};
        while(1){
            pthread_mutex_lock(&id_lock); //fetch unique id for customer
            id = new_id;
            new_id++;
            pthread_mutex_unlock(&id_lock);
        
            std::cout << "Customer " << id << " is leaving for barber shop\n";
            randWait(5);
            std::cout << "Customer " << id << " arrived at barber shop.\n";

            if(sem_trywait(&waitingRoom) == -1){
                std::cout << "Waiting room full. Customer" << id << " is leaving.\n";
            }
            else{
            std::cout << "Customer " << id << " is entering waiting room.\n";

            sem_wait(&barberChair);
            sem_post(&waitingRoom);
        
            std::cout << "Customer " << id << " is waking the barber.\n";
            sem_post(&barberPillow);
            sem_wait(&seatBelt);
            sem_post(&barberChair);
            std::cout<< "Customer " << id << " is leaving the barber shop.\n";
            }
        }
    }

    void *barber(void *unused){
        while(1){ //a workaholic!
            std::cout<<"The barber is sleeping.\n";
            sem_wait(&barberPillow);

            std::cout <<"The barber is cutting hair\n";
            randWait(3);
            std::cout <<"The barber has finished cutting hair.\n";
            
            sem_post(&seatBelt);
        }
    }
    
    void randWait(int seconds){
        std::chrono::milliseconds timespan(1195);
        std::this_thread::sleep_for(timespan);
    }
