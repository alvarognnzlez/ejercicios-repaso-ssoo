#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>


int t1, cap, t2, k, n;
// cinta transportadora
sem_t sem_libres; // espacios libres en la cinta
sem_t sem_listos  // productos libres para inspeccionar
    int enviados = 0;
int caja = 0;
//

void *manejador_inspectores(void *arg) {
  id = *(int *)arg;
  while (1) {
    // Comprobamos si se completaron todos los envíos antes de volver a producir
    pthread_mutex_lock(&mutex);
    if (enviados >= n) {
      pthread_mutex_unlock(&mutex);
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);

    sem_wait(&sem_libres); // esperamos a que haya espacio en la cinta
    // Comprobamodde nuevo tras despertar (puede que hayan terminado)

    pthread_mutex_lock(&mutex);
    if (enviado >= n) {
      pthread_mutex_unlock(&mutex) sem_post(&sem_libres);
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);

    // ensanblamos la pieza
    printf("Ensamblador %d, ensamblando pieza", id);
    sleep(rand() % 2);

    // despositamos en la cinta
    printf("Ensamblador %d, depositando pieza", id);
    sem_post(&sem_listos);
  }
}

void *manejador_t2(void *arg) {
  int id = *(int *)arg;

  while (1) { // esperamos a que haya productos en la cinta
    sem_wait(&sem_listos);

    // comprobamos si se han hecho todos lso envios y debemos terminar
    pthread_mutex_lock(&mutex);
    if (enviados >= n) {
      pthread_mutex_unlock(&mutex);
      sem_post(&sem_listos); // dejamos el token para que otro inspector salga
      pthread_exit(NULL);
    }
    pthread_mutex_lock(&mutex);

    // liberamos espacio en la cinta
    sem_post(&sem_libres);

    // inspeccionamos

    printf("Inspeccionador %d, inspeccionando producto", id);
    sleep(rand() % 2);
    if (rand() % 10 < 7) {
      printf("Producto APROBADO, va a la caja\n", id);
      // lo tenemos que meter a la caja;
      pthread_mutex_lock(&mutex);
      caja++;
      printf("Inspector %d, Caja:%d/%d", id, caja, k);

      // ahora comprobamos si la caja esta llena
      if (caja >= k) {
        pthread_cond_signal(&cond_envio);
      }
      pthread_mutex_unlock(&mutex);
    } else {
      printf("Producto rechazado");
    }
  }
}

void *funcion_gestor(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    // esperamos a que la caja este llena para viajar;
    while (caja < k && enviados < n) {
      pthread_cond_wait(&cond_envio, &mutex);
    }
    // condicion de parada
    if (enviados >= n) {
      pthread_mutex_unlock(&mutex);
      pthread_exit(NULL);
    }
    // si llega aqui significa que todavia puede hacer otro envio, lo hacemos
    enviados++;
    caja = 0;
    printf("Envio realizado,caja reseteada");
    pthread_mutex_unlock(&mutex);

    sleep(rand() % 2);
  }
}

// Caja de envio
pthread_mutex_t mutex;
pthread_cond_t cond_envio;

int main(int argc, char **argv) {

  if (argc != 6) {
    printf("Uso: %s <t1>, <t2> , <cap>, <k>,<n>");
    exit(1);
  }

  t1 = atoi(argv[1]);
  t2 = atoi(argv[2]);
  cap = atoi(argv[3]);
  k = atoi(argv[4]);
  n = atoi(argv[5]);
  // inicializamos primero semaforos siempre
  sem_init(&sem_libres, 0, cap);
  sem_init(&sem_listos, 0);

  // inicializamos el mutex y condicion
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_envio, NULL);

  pthread_t gestor;
  pthread_t *ensambladores = (pthread_t *)malloc(sizeof(pthread_t) * t1);
  pthread_t *inspectores = (pthread_t *)malloc(sizeof(pthread_t) * t2);

  int *ids1 = (int *)malloc(sizeof(int) * t1);

  int *ids2 = (int *)malloc(sizeof(int) * t2);

  // CREAMOS THREAD GESTOR
  pthread_create(&gestor, NULL, funcion_gestor, NULL);

  // CREAMOS THREAD ENSAMBLADOR
  for (int i = 0; i < t1; i++) {
    ids1[i] = i;
    pthread_create(&ensambladores[i], NULL, funcion_ensamblador, &ids[i]);
  }
  // CREAMOS THREAD INSPECTORES
  for (int i = 0; i < t2; i++) {
    ids2[i] = i;
    pthread_create(&inspectores[i], NULL, manejador_inspectores, NULL);
  }

  // y los eliminamos
  pthread_join(gestor, NULL);

  // despertamos los hilos que estuvieran bloqueados para que salgan con exit
  for (int i = 0; i < t1; i++) {
    sem_post(&sem_listos);
  }
  for (int i = 0; i < t2; i++) {
    sem_post(&sem_libres);
  }

  for (int i = 0; i < t1; i++) {
    pthread_join(ensambladores[i], NULL);
  }
  for (int i = 0; i < t2; i++) {
    pthread_join(inspectores[i], NULL);
  }

  // destruimos semaforos y mutex
  sem_destroy(&sem_listos);
  sem_destroy(&sem_libres);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destoy(&cond_envio);

  free(ensambladores);
  free(inspectores);
  free(ids1);
  free(ids2);

  return 0;
}