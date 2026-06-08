#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5
#define TANDAS 20

//variables que usemos tanto en main comom en el manejador van fuera del main

sem_t palillos[N]; //declaramos los semaforos que vamos a usar
pthread_mutex_t mutex;
pthread_cond_t arroz_listo; //main avisa que hay arroz
int porciones=0;

void* manejador_hilos(void* arg){
    id = *(int*) arg; //desreferenciamos el argumento que nos pasa, que es el id del filosofo que come
    int izq = id;
    int der (id+1) % N;
    
    pthread_mutex_locj(&mutex);
    while(porciones==0){ //mientras que no haya porciones suficientes, esperamos a la señal de que hay porciones (la activa el main)
        pthread_cond_wait(&arroz_listo);
    }
    porciones--;
    pthread_mutex_unlock(&mutex);

    if(id == N-1){
        sem_wait(&palillos[der]);
        sem_wait(&palillos[izq]);
    }else{
        sem_wait(&palillos[izq]);
        sem_wait(&palillos[der]);
    }

    printf("Filosofo %d comiendo\n",id);
    sleep(rand()%5+1);
    sem_post(&palillos[der]);
    sem_post(&palillos[izq]);

    printf("Filosofo %d terminó\n")
    return NULL;

}
int main(int argc, char** argv){
    pthread_t hilos[N];
    int ids[N];
    srand(time(NULL));

    for(int i=0;i<N;i++){
        ids[i]=i;
        sem_init(&palillos[i],0,1); //lo iniciamos a 1, listo para cogerlo
    }

    for(t=0;t<TANDAS;t++){

        for(int i=0;i<N;i++){
            ids[i]=i;
            pthread_create(&hilos[i],NULL,filosofo,&ids[i]);


            printf("Preparando arroz\n");
            sleep(2);
            //bloqueamos mutex para indicar que hay arroz disponible para comer
            pthread_mutex_lock(&mutex);
            porciones=N; //creamos 5 porciones nuevas, tenemos que enviar una señal a cada hilo para que sepa que ya puede comer
            for(int i=0;i<porciones;i++){
                pthread_cond_signal(&arroz_listo);
            }
            pthread_mutex_unlock(&mutex); //desbloqueamos el mutex


            //ahora esperamos a que acaben los hilos de comer y destruimos todo

            for(int i=0;i<N;i++){
                phtread_join(hilos[i],NULL);
            }

            printf("Tanda completada \n");
        }

    }
    for(int i=0;i<N;i++){
        sem_destroy(&palillos[i]);
    }
    return 0;
}