#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int t1, cap, t2, k, n;
// cinta transportadora
sem_t sem_libres     // espacios libres en la cinta
    sem_t sem_listos // productos libres para inspeccionar

    void *manejador_t1(void *arg) {
  id = *(int *)arg;
}

void *manejador_t2(void *arg) {}

// Caja de envio
pthread_mutex_t mutex;
pthread_cond_t cond_envio;

int main(int argc, char **argv) {

  if (argc != 5) {
    printf("Uso: %s <t1>, <t2> , <cap>, <k>,<n>");
    exit(1);
  }

  t1 = atoi(argv[1]);
  t2 = atoi(argv[2]);
  cap = atoi(argv[3]);
  k = atoi(argv[4]);
  n = atoi(argv[5]);

  pthread_t threadK;
  pthread_t *threadT1 = (pthread_t *)malloc(sizeof(pthread_t) * t1);
  pthread_t *threadT2 = (pthread_t *)malloc(sizeof(pthread_t) * t2);

  int *ids1 = (int *)malloc(sizeof(int) * t1);

  int *ids2 = (int *)malloc(sizeof(int) * t2);

  // ahorae inicio todos los semaforos

  free(threadT1);
  free(threadT2);
  free(ids1);
  free(ids2);

  return 0;
}