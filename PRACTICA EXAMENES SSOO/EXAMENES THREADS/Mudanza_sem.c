#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

volatile int tanda;
int nTandas;
int n;
int m;
int t1;
int t2;
volatile int cajasFurgo;

sem_t sem_piso;
sem_t sem_descansillo;

pthread_mutex_t mutex;
pthread_cond_t furgoLLena;


void * func_eqi1(void * arg) {
    int id = *(int*) arg;
    while (1) {
        sem_wait(&sem_piso);
        //Lo hacemos aqui para que no se impriman los mensajes al despertarse.
        // si la ponemos en el while en la ultima iteración cada uno de los hilos escribira un mensaje que no corresponde.
        if (tanda == nTandas) {
            pthread_exit(0);
        }

        printf("[Equipo 1, personna %d] Bajando al descansillo caja\n", id);
        sleep(rand() % 2);
        sem_post(&sem_descansillo);
    }
}

void* func_eqi2(void * arg) {
    int id = *(int*) arg;
    while (1) {
        sem_wait(&sem_descansillo);

        if (tanda == nTandas) {
            pthread_exit(0);
        }

        printf("[Equipo 2, personna %d] Subiendo caja\n", id);
        sleep(rand() % 2);
        pthread_mutex_lock(&mutex);
        cajasFurgo++;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&furgoLLena);
    }
}

void * func_transpor () {
    while (tanda < nTandas) {
        pthread_mutex_lock(&mutex);
        while (cajasFurgo < m) {
            pthread_cond_wait(&furgoLLena, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        printf("[Trasnportista] Realizado viaje\n");
        sleep(rand() % 5);
        printf("[Trasnportista] Volviendo\n");
        // actualizamos tanda
        tanda++;
        cajasFurgo = 0;
        for (int i = 0; i < m; i++) {
            sem_post(&sem_piso);
        }
    }
    // Abrir semaforos para que acaben todos los threads.
    for (int i = 0; i < t1 - m; i++) sem_post(&sem_piso);
    for (int i = 0; i < t2; i++) sem_post(&sem_descansillo);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    nTandas = n/m;
    t1 = atoi(argv[3]);
    t2 = atoi(argv[4]);
    tanda = 0;


    pthread_t  threadTranspor;
    pthread_t* threads1 = (pthread_t *) malloc(sizeof(pthread_t) * t1);
    pthread_t* threads2 = (pthread_t *) malloc(sizeof(pthread_t) * t2);
    int* ids1 = (int *) malloc(sizeof(int) * t1);
    int* ids2 = (int *) malloc(sizeof(int) * t2);

    sem_init(&sem_piso, 0, m);
    sem_init(&sem_descansillo, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < t1; i++) {
        ids1[i] = i;
        pthread_create(&threads1[i], NULL, func_eqi1, &ids1[i] );
    }
    for (int i = 0; i < t2; i++) {
        ids2[i] = i;
        pthread_create(&threads2[i], NULL, func_eqi2, &ids2[i] );
    }

    pthread_create(&threadTranspor, NULL, func_transpor, NULL);

    for (int i = 0; i < t1; i++) {
        pthread_join(threads1[i], NULL);
    }

    for (int i = 0; i < t2; i++) {
        pthread_join(threads2[i], NULL);
    }

    pthread_join(threadTranspor, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem_piso);
    sem_destroy(&sem_descansillo);
    pthread_cond_destroy(&furgoLLena);

    free(threads1);
    free(threads2);
    free(ids1);
    free(ids2);
    return 0;
}
