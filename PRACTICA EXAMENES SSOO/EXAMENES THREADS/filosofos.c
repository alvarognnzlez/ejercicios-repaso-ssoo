#include <semaphore.h> //vamos a necesitar 5 semaforos, 1 por filosofo
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 5
sem_t palillos[N];

void* filosofos(void* arg){
    int id = *(int*) arg; //id del filoso
    int izq = id;
    int der = (id+1) % N;


    wihle(1){
        printf("Filosofo %d pensando\n",id);
        fflush(stdout);
        sleep(rand()%5+1);

        printf("Filosofo %d quiere comer\n",id);
        fflush(stdout);
        sleep(rand()%5+1);

        if(id==N-1){ //si es el ultimo filosofo entonces primero coge el de la derecha y luego el de la izquierda
            sem_wait(&palillos[der]);
            sem_wait(&palillos[izq]);
        }else{
            sem_wait(&palillos[izq]);
            sem_wait(&palillos[der]);
        }
        printf("Filosofo %d comiendo\n",id);
        fflush(stdout);
        sleep(rand() % 5+1);

        sem_post(&palillos[izq]);
        sem_post(&palillos[der]);
    }
    return NULL;
}


int main(int argc, char** argv){
pthread_t hilos[5];
int ids[5];
srand(time(NULL));


//PRIMERO CREAMOS LOS SEMAFOROS
for(int i=0;i<N;i++){
    sem_init(&palillos[i],0,1);
    ids[i]=i;
}


for(int i=0; i<N; i++){
    ids[i]=i;
    pthread_create(&hilos[i],NULL,filosofo,&ids[i]);
}

while(1){
    sleep(100);
}
return 0;
}