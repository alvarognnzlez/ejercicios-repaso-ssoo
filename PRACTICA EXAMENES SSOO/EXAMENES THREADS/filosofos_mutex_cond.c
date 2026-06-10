#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 5
#define TANDAS 20

// variables que usemos tanto en main como en el manejador van fuera del main

sem_t           palillos[N]; // declaramos los semaforos que vamos a usar
pthread_mutex_t mutex;
pthread_cond_t  arroz_listo; // main avisa que hay arroz
int             porciones = 0;

void *manejador_hilos(void *arg) {
  int id  = *(int *)arg; // ← faltaba "int"
  int izq = id;
  int der = (id + 1) % N; // ← faltaba el "="

  pthread_mutex_lock(&mutex); // ← typo "pthread_mutex_locj"
  while (porciones == 0) {    // mientras no haya porciones, esperamos
    pthread_cond_wait(&arroz_listo, &mutex); // ← faltaba el segundo argumento &mutex
  }
  porciones--;
  pthread_mutex_unlock(&mutex);

  // Evitar deadlock: el último filósofo coge los palillos en orden inverso
  if (id == N - 1) {
    sem_wait(&palillos[der]);
    sem_wait(&palillos[izq]);
  } else {
    sem_wait(&palillos[izq]);
    sem_wait(&palillos[der]);
  }

  printf("Filosofo %d comiendo\n", id);
  sleep(rand() % 5 + 1);
  sem_post(&palillos[der]);
  sem_post(&palillos[izq]);

  printf("Filosofo %d terminó\n", id); // ← faltaba ; y el argumento id
  return NULL;
}

int main(int argc, char **argv) {
  pthread_t hilos[N];
  int       ids[N];
  srand(time(NULL));

  // Inicializar mutex y condición — ANTES de crear hilos
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&arroz_listo, NULL);

  for (int i = 0; i < N; i++) {
    ids[i] = i;
    sem_init(&palillos[i], 0, 1); // lo iniciamos a 1, listo para cogerlo
  }

  for (int t = 0; t < TANDAS; t++) { // ← "t" no estaba declarado

    // Crear todos los hilos de la tanda
    for (int i = 0; i < N; i++) { // ← el resto del bucle estaba mal anidado dentro
      ids[i] = i;
      pthread_create(&hilos[i], NULL, manejador_hilos, &ids[i]); // ← "filosofo" no existe
    }

    printf("Preparando arroz\n");
    sleep(2);

    // Indicar que hay arroz disponible
    pthread_mutex_lock(&mutex);
    porciones = N;
    for (int i = 0; i < N; i++) {
      pthread_cond_signal(&arroz_listo); // señal a cada filósofo
    }
    pthread_mutex_unlock(&mutex);

    // Esperar a que todos los filósofos de esta tanda terminen
    for (int i = 0; i < N; i++) {
      pthread_join(hilos[i], NULL); // ← typo "phtread_join"
    }

    printf("Tanda %d completada\n", t + 1);
  }

  // Destruir herramientas
  for (int i = 0; i < N; i++) {
    sem_destroy(&palillos[i]);
  }
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&arroz_listo);

  return 0;
}