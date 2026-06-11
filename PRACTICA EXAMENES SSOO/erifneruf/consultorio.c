/*
 * consultorio.c - Problema del consultorio médico
 *
 * Escenario:
 *   - Un médico atiende pacientes de uno en uno.
 *   - La sala de espera tiene capacidad limitada (MAX_SALA sillas).
 *   - Si la sala está llena, el paciente se va sin esperar.
 *   - El médico atiende mientras haya pacientes; si no hay, duerme.
 *
 * Compilar: gcc -o consultorio consultorio.c -lpthread
 * Ejecutar: ./consultorio
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_SALA     5    /* sillas en la sala de espera */
#define N_PACIENTES  10   /* número total de pacientes */

/* ---- Variables compartidas ---- */
int pacientes_en_sala = 0;

/* ---- Primitivas de sincronización ---- */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* protege pacientes_en_sala */
sem_t sala_libre;   /* semáforo: cuántas sillas quedan libres  */
sem_t hay_paciente; /* semáforo: avisa al médico de que hay alguien */

/* ---- Hilo PACIENTE ---- */
void *hilo_paciente(void *arg) {
    int id = *(int *)arg;
    free(arg);

    /* Intentar entrar a la sala (sin bloqueo, por si está llena) */
    if (sem_trywait(&sala_libre) != 0) {
        printf("Paciente %2d: sala llena, se va sin esperar.\n", id);
        return NULL;
    }

    /* Entrar a la sala */
    pthread_mutex_lock(&mutex);
    pacientes_en_sala++;
    printf("Paciente %2d: entra a la sala.  En sala: %d\n", id, pacientes_en_sala);
    pthread_mutex_unlock(&mutex);

    /* Avisar al médico */
    sem_post(&hay_paciente);

    return NULL;
}

/* ---- Hilo MÉDICO ---- */
void *hilo_medico(void *arg) {
    int atendidos = 0;

    while (atendidos < N_PACIENTES) {
        sem_wait(&hay_paciente); /* esperar a que llegue un paciente */

        pthread_mutex_lock(&mutex);
        pacientes_en_sala--;
        printf("Médico    : atendiendo paciente. Quedan en sala: %d\n",
               pacientes_en_sala);
        pthread_mutex_unlock(&mutex);

        sem_post(&sala_libre); /* liberar la silla que ocupaba el paciente */

        sleep(1); /* simular tiempo de consulta */
        atendidos++;
    }

    printf("Médico    : ha atendido a todos. Cerrando consulta.\n");
    return NULL;
}

/* ---- main ---- */
int main(void) {
    pthread_t medico;
    pthread_t pacientes[N_PACIENTES];

    /* Inicializar semáforos */
    sem_init(&sala_libre,   0, MAX_SALA); /* hay MAX_SALA sillas libres al inicio */
    sem_init(&hay_paciente, 0, 0);        /* nadie ha llegado aún                 */

    /* Crear hilo médico */
    pthread_create(&medico, NULL, hilo_medico, NULL);

    /* Crear hilos paciente con pequeño retardo entre llegadas */
    for (int i = 0; i < N_PACIENTES; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&pacientes[i], NULL, hilo_paciente, id);
        usleep(300000); /* 300 ms entre llegadas */
    }

    /* Esperar a todos los pacientes */
    for (int i = 0; i < N_PACIENTES; i++) {
        pthread_join(pacientes[i], NULL);
    }

    /* Esperar al médico */
    pthread_join(medico, NULL);

    /* Liberar recursos */
    sem_destroy(&sala_libre);
    sem_destroy(&hay_paciente);
    pthread_mutex_destroy(&mutex);

    return 0;
}
