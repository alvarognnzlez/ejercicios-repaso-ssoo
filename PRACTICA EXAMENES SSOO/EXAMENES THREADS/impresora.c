#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int             N; // NUMERO DE usuarios
sem_t           sem_sala; // 3 puestos de preparación
pthread_mutex_t mutex;
pthread_cond_t  cond_usuarios; // en esta condicion los usuarios esperan para imprimir
pthread_cond_t  cond_tecnico;  // el tecnico espera en esta condicion ← faltaba ;

int impresiones         = 0; // maximo de 5 impresiones hasta que haya un mantenimiento
int mantenimiento_pendiente = 0; // ← typo "manteniemto" → "mantenimiento"
int impresora_libre     = 1; // la impresora al principio esta libre


void *manejador_usuarios(void *arg) {
  int id = *(int *)arg; // ← faltaba ;
  while (1) {
    // lo primero que hacemos es coger un puesto de impresion, maximo hay 3
    sem_wait(&sem_sala); // ← faltaba ;
    printf("Usuario %d: Preparando documento\n", id);
    sleep(rand() % 3);
    sem_post(&sem_sala); // ← faltaba ;

    // esperamos que no haya mantenimiento y que la impresora este libre
    pthread_mutex_lock(&mutex);
    while (mantenimiento_pendiente || !impresora_libre) { // ← typo corregido
      pthread_cond_wait(&cond_usuarios, &mutex);
    }

    // reservamos la impresora
    impresora_libre = 0;
    pthread_mutex_unlock(&mutex);

    // imprimimos
    printf("Usuario %d. Imprimiendo documento...\n", id);
    sleep(rand() % 2);

    // liberamos la impresora y contamos la impresion
    pthread_mutex_lock(&mutex);
    impresiones++;
    printf("Impresión realizada\n");
    impresora_libre = 1;

    if (impresiones % 5 == 0) {
      mantenimiento_pendiente = 1;
      printf("Mantenimiento necesario.\n"); // ← faltaba ;
      pthread_cond_signal(&cond_tecnico);   // ← era "cond_mantenimiento" (no existe)
    } else {
      // Como no hay mantenimiento necesario, despertamos un usuario en cadena
      pthread_cond_signal(&cond_usuarios);
    }
    pthread_mutex_unlock(&mutex); // ← era pthread_mutex_LOCK (¡deadlock!)
  }
  pthread_exit(NULL);
}


void *manejador_tecnico(void *arg) { // ← añadido void*arg aunque no se use
  while (1) {
    pthread_mutex_lock(&mutex);
    // si hay mantenimiento ningun usuario puede imprimir.
    while (!mantenimiento_pendiente) { // esperamos a que sea necesario mantenimiento
      pthread_cond_wait(&cond_tecnico, &mutex);
    }
    impresora_libre = 0; // reservamos la impresora para hacer el mantenimiento
    pthread_mutex_unlock(&mutex);

    printf("Realizando mantenimiento\n");
    sleep(rand() % 2);
    printf("Mantenimiento Realizado\n");

    // restablecemos todo y despertamos a un usuario
    pthread_mutex_lock(&mutex);
    mantenimiento_pendiente = 0;
    impresora_libre         = 1;
    pthread_cond_signal(&cond_usuarios); // ← era (&cond_usuario, &mutex): nombre mal y arg extra
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Uso: %s N_Users\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  N = atoi(argv[1]);
  if (N <= 0) {
    fprintf(stderr, "Error, N debe ser mayor que 0\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));

  // inicializamos primero siempre los semaforos
  sem_init(&sem_sala, 0, 3);
  // ahora los mutex y condiciones
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_usuarios, NULL); // ← faltaba NULL
  pthread_cond_init(&cond_tecnico, NULL);  // ← faltaba NULL

  // reservamos memoria
  pthread_t  tecnico;
  pthread_t *usuarios = (pthread_t *)malloc(sizeof(pthread_t) * N);
  int       *ids      = (int *)malloc(sizeof(int) * N);

  // creamos los hilos
  pthread_create(&tecnico, NULL, manejador_tecnico, NULL);

  for (int i = 0; i < N; i++) {
    ids[i] = i;
    pthread_create(&usuarios[i], NULL, manejador_usuarios, &ids[i]);
  }

  pthread_join(tecnico, NULL);

  for (int i = 0; i < N; i++) {
    pthread_join(usuarios[i], NULL); // ← era "usuarios" sin [i]
  }

  sem_destroy(&sem_sala);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_usuarios);
  pthread_cond_destroy(&cond_tecnico);

  free(usuarios);
  free(ids);
  return 0;
}