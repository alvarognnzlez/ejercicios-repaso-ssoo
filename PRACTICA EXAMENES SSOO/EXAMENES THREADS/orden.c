#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>   // ← faltaba este include
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *manejador_hilo1(void *arg) {
  // este es el encargado de escribir primero
  printf("Soy el hilo1, escribo primero\n");
  sem_post(&sem);
  pthread_exit(NULL);
}

void *manejador_hilo2(void *arg) {
  // este es el encargado de escribir segundo, primero sem_wait y luego el print
  sem_wait(&sem);
  printf("Soy el hilo2, escribo segundo\n"); // ← faltaba ;
  pthread_exit(NULL);
}

int main() {
  pthread_t hilo1, hilo2;

  if (sem_init(&sem, 0, 0) != 0) {
    perror("Error al inicializar el semaforo");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&hilo1, NULL, manejador_hilo1, NULL) != 0) {
    perror("Error al crear el hilo 1");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&hilo2, NULL, manejador_hilo2, NULL) != 0) { // ← era manejador_hilo1 (¡bug!)
    perror("Error al crear el hilo 2");
    exit(EXIT_FAILURE);
  }

  // una vez creados, eliminamos los threads y luego los semaforos
  pthread_join(hilo1, NULL);
  pthread_join(hilo2, NULL);

  sem_destroy(&sem);

  return 0;
}
