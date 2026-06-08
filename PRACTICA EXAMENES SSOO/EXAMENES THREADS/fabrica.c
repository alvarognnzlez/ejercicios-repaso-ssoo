/*
 * EJERCICIO: Cadena de Montaje
 *
 * Uso: ./fabrica <t1> <t2> <cap> <k> <n>
 *
 *   t1  -> numero de ensambladores (productores)
 *   t2  -> numero de inspectores   (consumidores)
 *   cap -> capacidad maxima de la cinta (buffer)
 *   k   -> productos aprobados para realizar un envio
 *   n   -> numero total de envios antes de terminar
 *
 * Sincronizacion:
 *   - sem_libres  : espacios libres en la cinta  (init = cap)
 *   - sem_listos  : productos disponibles en cinta (init = 0)
 *   - mutex + cond_envio : protegen la caja de envio
 *   - enviados    : contador de envios realizados (condicion de parada)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* ── Variables globales ─────────────────────────────────────────── */

int t1, t2, cap, k, n;

/* Cinta transportadora (buffer) */
sem_t sem_libres;   /* espacios libres en la cinta */
sem_t sem_listos;   /* productos listos para inspeccionar */

/* Caja de envio */
pthread_mutex_t mutex;
pthread_cond_t  cond_envio;
int caja = 0;       /* productos aprobados acumulados */
int enviados = 0;   /* envios realizados */

/* ── Ensamblador (Productor) ────────────────────────────────────── */

void* funcion_ensamblador(void* arg) {
    int id = *(int*)arg;

    while (1) {
        /* Comprobamos si ya se completaron todos los envios ANTES de producir */
        pthread_mutex_lock(&mutex);
        if (enviados >= n) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex);

        /* Esperar a que haya espacio en la cinta */
        sem_wait(&sem_libres);

        /* Volvemos a comprobar tras despertar (puede que hayan terminado) */
        pthread_mutex_lock(&mutex);
        if (enviados >= n) {
            pthread_mutex_unlock(&mutex);
            sem_post(&sem_libres); /* devolvemos el hueco */
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex);

        /* Ensamblar pieza (tiempo aleatorio) */
        printf("[Ensamblador %d] Ensamblando pieza...\n", id);
        sleep(rand() % 3);

        /* Depositar en la cinta */
        printf("[Ensamblador %d] Pieza depositada en la cinta.\n", id);
        sem_post(&sem_listos);
    }
}

/* ── Inspector (Consumidor) ─────────────────────────────────────── */

void* funcion_inspector(void* arg) {
    int id = *(int*)arg;

    while (1) {
        /* Esperar a que haya un producto en la cinta */
        sem_wait(&sem_listos);

        /* Comprobar si debemos terminar */
        pthread_mutex_lock(&mutex);
        if (enviados >= n) {
            pthread_mutex_unlock(&mutex);
            sem_post(&sem_listos); /* dejamos el token para que otro inspector salga */
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex);

        /* Liberar espacio en la cinta */
        sem_post(&sem_libres);

        /* Inspeccionar (tiempo aleatorio) */
        printf("[Inspector  %d] Inspeccionando producto...\n", id);
        sleep(rand() % 2);

        /* 70% de probabilidad de aprobar */
        if (rand() % 10 < 7) {
            printf("[Inspector  %d] Producto APROBADO -> va a la caja.\n", id);

            pthread_mutex_lock(&mutex);
            caja++;
            printf("[Inspector  %d] Caja: %d/%d\n", id, caja, k);

            /* Avisar al gestor si la caja esta llena */
            if (caja >= k) {
                pthread_cond_signal(&cond_envio);
            }
            pthread_mutex_unlock(&mutex);
        } else {
            printf("[Inspector  %d] Producto RECHAZADO -> descartado.\n", id);
        }
    }
}

/* ── Gestor de envios ───────────────────────────────────────────── */

void* funcion_gestor(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        /* Esperar hasta que la caja tenga k productos */
        while (caja < k && enviados < n) {
            pthread_cond_wait(&cond_envio, &mutex);
        }

        /* Condicion de parada */
        if (enviados >= n) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        /* Realizar envio */
        enviados++;
        caja = 0;
        printf("\n[Gestor] *** ENVIO %d/%d realizado! Caja reseteada. ***\n\n",
               enviados, n);

        /* Si ya se completaron todos los envios, despertar a todos */
        if (enviados >= n) {
            /* Despertar al propio gestor en su siguiente iteracion */
            pthread_cond_broadcast(&cond_envio);
        }

        pthread_mutex_unlock(&mutex);

        /* Simular tiempo de gestion del envio */
        sleep(rand() % 2);
    }
}

/* ── Main ───────────────────────────────────────────────────────── */

int main(int argc, char** argv) {
    if (argc != 6) {
        fprintf(stderr, "Uso: %s <t1> <t2> <cap> <k> <n>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    t1  = atoi(argv[1]);
    t2  = atoi(argv[2]);
    cap = atoi(argv[3]);
    k   = atoi(argv[4]);
    n   = atoi(argv[5]);

    srand(42);

    /* Inicializar semaforos */
    sem_init(&sem_libres, 0, cap);  /* cap espacios libres al inicio */
    sem_init(&sem_listos, 0, 0);    /* 0 productos al inicio */

    /* Inicializar mutex y variable de condicion */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_envio, NULL);

    /* Reservar memoria dinamica para hilos e ids */
    pthread_t  gestor;
    pthread_t* ensambladores = (pthread_t*)malloc(sizeof(pthread_t) * t1);
    pthread_t* inspectores   = (pthread_t*)malloc(sizeof(pthread_t) * t2);
    int*       ids1          = (int*)malloc(sizeof(int) * t1);
    int*       ids2          = (int*)malloc(sizeof(int) * t2);

    /* Crear hilo gestor */
    pthread_create(&gestor, NULL, funcion_gestor, NULL);

    /* Crear ensambladores */
    for (int i = 0; i < t1; i++) {
        ids1[i] = i;
        pthread_create(&ensambladores[i], NULL, funcion_ensamblador, &ids1[i]);
    }

    /* Crear inspectores */
    for (int i = 0; i < t2; i++) {
        ids2[i] = i;
        pthread_create(&inspectores[i], NULL, funcion_inspector, &ids2[i]);
    }

    /* Esperar al gestor primero (el que controla la terminacion) */
    pthread_join(gestor, NULL);

    /* Despertar a ensambladores e inspectores bloqueados en semaforos */
    for (int i = 0; i < t1; i++) sem_post(&sem_libres);
    for (int i = 0; i < t2; i++) sem_post(&sem_listos);

    /* Join de ensambladores e inspectores */
    for (int i = 0; i < t1; i++) pthread_join(ensambladores[i], NULL);
    for (int i = 0; i < t2; i++) pthread_join(inspectores[i],   NULL);

    /* Destruir recursos */
    sem_destroy(&sem_libres);
    sem_destroy(&sem_listos);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_envio);

    free(ensambladores);
    free(inspectores);
    free(ids1);
    free(ids2);

    printf("[Main] Programa terminado. %d envios completados.\n", n);
    return 0;
}