/*
    sleepingbbq.cpp
    by thcloak
    
    Compile with:
    g++ -Wall ./sleepingbbq.cpp -o sleepingbbq -pthread -std=c++11

    Known issues:
    - sem waitingRoomQueue[numChairs] is incremented indefinitely.
        Might break something in the long run :/
    - It is possible to have all customer threads wait simultaneously,
        preventing new customers from leaving for barber shop.
    Roadmap:
    - Impelement traffic load factor, passed as argument. Should make it
        easier to adjust barber sleepiness.

        This well-commented implementation helped me get
        get started on the problem.
        https://github.com/aseempatni/os/blob/master/5_semaphore/sleepBarber.c
*/

#include <pthread.h>
#include <semaphore.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <iostream>

enum messageType{
    leave_for_barbershop,
    arrive_at_barbershop,
    waiting_room_full,
    enter_waiting_room,
    check_on_barber,
    leave_successfully
};

void customerMessage(int id, messageType message);
void *customer(void *numChairs);
void *barber(void *);
void randWait(int approxSeconds);

sem_t* waitingRoomQueue;
sem_t seatsAvailable;
sem_t barberChairEmpty;
sem_t barberPillow;
sem_t seatBelt;
pthread_mutex_t room_state_change;


//guards critical section where customer() pulls
//a unique id from global counter new_id.
pthread_mutex_t id_lock;
int new_id{1}; 

int main(int argc, char* argv[]){
    pthread_mutex_init(&id_lock, NULL);
    pthread_mutex_init(&room_state_change, NULL);

    int numChairs = atoi(argv[1]);
    long randSeed = atoi(argv[2]);

    pthread_t btid;
    pthread_t tid[5];

    if(argc!=3){
        std::cout << "Use: ./sleepingbbq <Num Chairs> <rand seed>\n";
        exit(-1); //fix segfault for fewer arguments
    }

    waitingRoomQueue = new sem_t[numChairs+1];

    std::cout  << "\n sleepingbbq.cpp\n\n";
    std::cout << "A joint solution to the sleeping barber problem and\n the producer\
 consumer problem, using semaphores.\n\n";

    std::this_thread::sleep_for(static_cast<std::chrono::milliseconds>(2000));

    srand(randSeed);

    for(int i = 0; i < numChairs + 1; i++){
        sem_init(&waitingRoomQueue[i], 0, 1);
    }
    
    sem_init(&seatsAvailable, 0, numChairs);
    sem_init(&barberChairEmpty, 0, 1);
    sem_init(&barberPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);

    pthread_create(&btid, NULL, barber, NULL);
    
    int thread_num{0};
    for (thread_num = 0; thread_num < 5; thread_num++) {
        pthread_create(&tid[thread_num], NULL, customer, (void*)&numChairs);
    }
    for (thread_num=0; thread_num<5; thread_num++) {
	    pthread_join(tid[thread_num],NULL);
    }

    pthread_join(btid, NULL);

    pthread_mutex_destroy(&id_lock);
    pthread_mutex_destroy(&room_state_change);
    delete [] waitingRoomQueue;
    waitingRoomQueue = NULL;
}

void *customer(void *numChairs){
    int* number_of_chairs = (int*)numChairs;
    int id{-1};
    int positionInQueue{*number_of_chairs - 1};
    while(1){
        pthread_mutex_lock(&id_lock); //fetch unique id for customer
            id = new_id;
            new_id++;
        pthread_mutex_unlock(&id_lock);
        
        customerMessage(id, leave_for_barbershop);
        randWait(10);
        customerMessage(id, arrive_at_barbershop);
                
        if(sem_trywait(&seatsAvailable) == -1){
            customerMessage(id, waiting_room_full);
        }
        else{
            customerMessage(id, enter_waiting_room);
            while(positionInQueue > 0){
                sem_wait(&waitingRoomQueue[positionInQueue]);
                sem_post(&waitingRoomQueue[positionInQueue+1]);
                positionInQueue--;
            }

            sem_wait(&barberChairEmpty);

            pthread_mutex_lock(&room_state_change);
                customerMessage(id, check_on_barber);
                sem_post(&waitingRoomQueue[1]);
                sem_post(&seatsAvailable);
                sem_post(&barberPillow);
            pthread_mutex_unlock(&room_state_change);
                
            sem_wait(&seatBelt);
            
            customerMessage (id, leave_successfully);
            sem_post(&barberChairEmpty);                
        }
    }
}

void *barber(void *unused){
    int isAsleep{-1};
    while(1){
        sem_getvalue(&barberPillow, &isAsleep);
        if(isAsleep == 0){
        std::cout<<"The barber is sleeping.\n";
        }
        sem_wait(&barberPillow);
        std::cout <<"The barber is cutting hair.\n";
        randWait(5);
        std::cout <<"The barber has finished cutting hair.\n";
            
        sem_post(&seatBelt);
        randWait(1);
    }
}
    
void randWait(int approxSeconds){
    std::this_thread::sleep_for(static_cast<std::chrono::milliseconds>
    (approxSeconds *  (rand() %  1000) ));
}

void customerMessage(int id, messageType message){
    std::stringstream stream;
    switch (message)
    {
        case leave_for_barbershop:
            stream << "Customer " << id << " is leaving for barber shop\n";
            break;
        case arrive_at_barbershop:
            stream << "Customer " << id << " arrived at barber shop.\n";
            break;
        case waiting_room_full:
            stream << "Waiting room full. Customer " << id << " IS LEAVING!\n";
            break;
        case enter_waiting_room:
            stream << "Customer " << id << " is entering waiting room.\n";
            break;
        case check_on_barber:
        //no need to clarify whether awake or asleep; the barber informing
        //about it is enough.
            stream << "Customer " << id << " is checking on the barber.\n";
            break;
        case leave_successfully:
        stream << "Customer " << id << " is leaving the barber shop.\n";
            break;
    }
    std::cout << stream.str();
    stream.str("");
    stream.clear();
    }