#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int n, m, t1, t2;
int tandas;
int numTanda;
pthread_mutex_t mutex;
sem_t           sem_descansillo;
sem_t           sem_piso;
int             cajasFurgo;
pthread_cond_t  furgoLlena;


void *funcion_eq1(void *arg) { // el argumento es el id de la persona
  int id = *(int *)arg;

  while (1) {
    sem_wait(&sem_piso); // restamos uno para que vaya haciendo m-1

    if (numTanda == tandas) { // repetimos hasta que numTanda == tandas
      pthread_exit(0);
    }
    printf("Equipo 1: persona %d Bajando al descansillo\n", id);
    sleep(rand() % 2);
    sem_post(&sem_descansillo); // ← typo ":;" → ";"
  }
}

void *funcion_eq2(void *arg) {
  int id = *(int *)arg;
  while (1) {
    sem_wait(&sem_descansillo); // ← faltaba ;
    if (numTanda == tandas) {
      pthread_exit(0);
    }
    printf("Equipo 2: persona %d Subiendo a la furgoneta\n", id);
    sleep(rand() % 2);
    pthread_mutex_lock(&mutex);
    cajasFurgo++;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&furgoLlena); // ← faltaba ;
  }
}


void *funcion_transportista(void *arg) {
  while (numTanda < tandas) { // viajamos hasta que se acaben las tandas
    pthread_mutex_lock(&mutex);
    while (cajasFurgo < m) { // la furgo lleva m cajas
      pthread_cond_wait(&furgoLlena, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("[Transportista] Realizando viaje\n");
    sleep(rand() % 2);
    printf("[Transportista] Volviendo del viaje\n");
    sleep(rand() % 2);

    // actualizamos la tanda cuando vuelve
    pthread_mutex_lock(&mutex);
    numTanda++;
    cajasFurgo = 0;
    pthread_mutex_unlock(&mutex);

    // reactivamos el equipo 1 para la siguiente ronda
    for (int i = 0; i < m; i++) {
      sem_post(&sem_piso);
    }
  }

  // Hemos completado todas las tandas: liberar hilos bloqueados
  for (int i = 0; i < t1; i++) {
    sem_post(&sem_piso);
  }
  for (int i = 0; i < t2; i++) {
    sem_post(&sem_descansillo);
  }

  return NULL;
}

int main(int argc, char **argv) {

  if (argc != 6) { // ← faltaba comprobación de argumentos; se necesitan 5 parámetros
    fprintf(stderr, "Uso: %s <n> <m> <t1> <t2> <tandas>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  n      = atoi(argv[1]); // ← typo "a0rgv" → "argv"
  m      = atoi(argv[2]);
  t1     = atoi(argv[3]);
  t2     = atoi(argv[4]);
  tandas = atoi(argv[5]); // ← "tandas" nunca se inicializaba

  // declaro threads e ids con memoria dinámica
  pthread_t  threadTranspor;
  pthread_t *threads1 = (pthread_t *)malloc(sizeof(pthread_t) * t1);
  pthread_t *threads2 = (pthread_t *)malloc(sizeof(pthread_t) * t2);
  int       *ids1     = (int *)malloc(sizeof(int) * t1);
  int       *ids2     = (int *)malloc(sizeof(int) * t2);

  // inicializo mutex, semaforos y condicion
  pthread_mutex_init(&mutex, NULL); // ← typo "&mutex;NULL" → "&mutex, NULL"
  pthread_cond_init(&furgoLlena, NULL); // ← faltaba completamente
  sem_init(&sem_descansillo, 0, 0);
  sem_init(&sem_piso, 0, m);

  // creo los hilos
  pthread_create(&threadTranspor, NULL, funcion_transportista, NULL);

  for (int i = 0; i < t1; i++) {
    ids1[i] = i;
    pthread_create(&threads1[i], NULL, funcion_eq1, &ids1[i]);
  }
  for (int i = 0; i < t2; i++) {
    ids2[i] = i;
    pthread_create(&threads2[i], NULL, funcion_eq2, &ids2[i]);
  }

  // espero a que terminen
  pthread_join(threadTranspor, NULL);

  for (int i = 0; i < t1; i++) {
    pthread_join(threads1[i], NULL);
  }
  for (int i = 0; i < t2; i++) {
    pthread_join(threads2[i], NULL);
  }

  // destruyo
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&furgoLlena); // ← faltaba completamente
  sem_destroy(&sem_piso);
  sem_destroy(&sem_descansillo);

  free(threads1);
  free(threads2);
  free(ids1);
  free(ids2);

  return 0;
}