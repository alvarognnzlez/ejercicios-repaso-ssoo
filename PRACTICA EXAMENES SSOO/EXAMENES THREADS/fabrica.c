#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int t1, cap, t2, k, n;

// cinta transportadora
sem_t sem_libres; // espacios libres en la cinta
sem_t sem_listos; // productos listos para inspeccionar  ← faltaba ;
int enviados = 0; // ← estaba pegado a la línea anterior
int caja = 0;

// Caja de envio — MOVIDO AQUÍ (antes estaban después de las funciones)
pthread_mutex_t mutex;
pthread_cond_t cond_envio;

void *manejador_ensamblador(void *arg) { // ← renombrado para claridad
  int id = *(int *)arg; // ← faltaba "int"
  while (1) {
    // Comprobamos si se completaron todos los envíos antes de volver a producir
    pthread_mutex_lock(&mutex);
    if (enviados >= n) {
      pthread_mutex_unlock(&mutex);
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);

    sem_wait(&sem_libres); // esperamos a que haya espacio en la cinta
    // Comprobamos de nuevo tras despertar (puede que hayan terminado)

    pthread_mutex_lock(&mutex);
    if (enviados >= n) { // ← typo "enviado" → "enviados"
      pthread_mutex_unlock(&mutex);
      sem_post(&sem_libres); // ← faltaba ; entre las dos llamadas
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);

    // ensamblamos la pieza
    printf("Ensamblador %d, ensamblando pieza\n", id);
    sleep(rand() % 2);

    // depositamos en la cinta
    printf("Ensamblador %d, depositando pieza\n", id);
    sem_post(&sem_listos);
  }
}

void *manejador_inspector(void *arg) { // ← renombrado para claridad
  int id = *(int *)arg;

  while (1) { // esperamos a que haya productos en la cinta
    sem_wait(&sem_listos);

    // comprobamos si se han hecho todos los envios y debemos terminar
    pthread_mutex_lock(&mutex);
    if (enviados >= n) {
      pthread_mutex_unlock(&mutex);
      sem_post(&sem_listos); // dejamos el token para que otro inspector salga
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex); // ← era pthread_mutex_lock (¡deadlock!)

    // liberamos espacio en la cinta
    sem_post(&sem_libres);

    // inspeccionamos
    printf("Inspector %d, inspeccionando producto\n", id);
    sleep(rand() % 2);
    if (rand() % 10 < 7) {
      printf("Producto APROBADO, va a la caja\n");
      // lo tenemos que meter a la caja
      pthread_mutex_lock(&mutex);
      caja++;
      printf("Inspector %d, Caja:%d/%d\n", id, caja, k);

      // ahora comprobamos si la caja esta llena
      if (caja >= k) {
        pthread_cond_signal(&cond_envio);
      }
      pthread_mutex_unlock(&mutex);
    } else {
      printf("Producto rechazado\n");
    }
  }
}

void *funcion_gestor(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    // esperamos a que la caja este llena para viajar
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
    printf("Envio realizado, caja reseteada\n");
    pthread_mutex_unlock(&mutex);

    sleep(rand() % 2);
  }
}

int main(int argc, char **argv) {

  if (argc != 6) {
    printf("Uso: %s <t1> <t2> <cap> <k> <n>\n", argv[0]);
    exit(1);
  }

  t1  = atoi(argv[1]);
  t2  = atoi(argv[2]);
  cap = atoi(argv[3]);
  k   = atoi(argv[4]);
  n   = atoi(argv[5]);

  // inicializamos primero semaforos siempre
  sem_init(&sem_libres, 0, cap);
  sem_init(&sem_listos, 0, 0); // ← faltaba el valor inicial (0)

  // inicializamos el mutex y condicion
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_envio, NULL);

  pthread_t  gestor;
  pthread_t *ensambladores = (pthread_t *)malloc(sizeof(pthread_t) * t1);
  pthread_t *inspectores   = (pthread_t *)malloc(sizeof(pthread_t) * t2);

  int *ids1 = (int *)malloc(sizeof(int) * t1);
  int *ids2 = (int *)malloc(sizeof(int) * t2);

  // CREAMOS THREAD GESTOR
  pthread_create(&gestor, NULL, funcion_gestor, NULL);

  // CREAMOS THREADS ENSAMBLADORES
  for (int i = 0; i < t1; i++) {
    ids1[i] = i;
    pthread_create(&ensambladores[i], NULL, manejador_ensamblador, &ids1[i]);
    // ← función y array corregidos (funcion_ensamblador no existía, &ids[i] tampoco)
  }
  // CREAMOS THREADS INSPECTORES
  for (int i = 0; i < t2; i++) {
    ids2[i] = i;
    pthread_create(&inspectores[i], NULL, manejador_inspector, &ids2[i]);
    // ← función corregida (era manejador_inspectores) y se pasa el id
  }

  // esperamos al gestor
  pthread_join(gestor, NULL);

  // despertamos los hilos bloqueados para que puedan hacer pthread_exit
  // ensambladores esperan en sem_libres → les hacemos post
  for (int i = 0; i < t1; i++) {
    sem_post(&sem_libres); // ← estaban invertidos: ponía sem_listos
  }
  // inspectores esperan en sem_listos → les hacemos post
  for (int i = 0; i < t2; i++) {
    sem_post(&sem_listos); // ← estaban invertidos: ponía sem_libres
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
  pthread_cond_destroy(&cond_envio); // ← typo "destoy" → "destroy"

  free(ensambladores);
  free(inspectores);
  free(ids1);
  free(ids2);

  return 0;
}