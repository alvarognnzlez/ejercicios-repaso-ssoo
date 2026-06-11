/*
 * consultorio.c - Problema del consultorio médico
 *
 * Escenario:
 *   - Hay p pacientes y 1 médico.
 *   - La sala de espera tiene s sillas.
 *   - Los pacientes llegan y esperan una silla; si no hay, se van.
 *   - El médico atiende de uno en uno cuando hay paciente esperando.
 *
 * Compilar: gcc -o consultorio consultorio.c -lpthread
 * Ejecutar: ./consultorio <num_pacientes> <num_sillas>
 *           Ej: ./consultorio 10 5
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* ---- Parámetros leídos de argv ---- */
int p, s; /* p = num pacientes, s = num sillas en sala */

/* ---- Variables compartidas ---- */
int pacientes_en_sala = 0;
int total_atendidos   = 0;

/* ---- Primitivas de sincronización ---- */
pthread_mutex_t mutex;
pthread_cond_t  hay_paciente; /* el médico espera aqui cuando no hay nadie */

/* ------------------------------------------------------------------ */
/*  Hilo PACIENTE                                                       */
/* ------------------------------------------------------------------ */
void *funcion_paciente(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex);

    /* Si la sala está llena, el paciente se va sin esperar */
    if (pacientes_en_sala >= s) {
        printf("Paciente %2d: sala llena, se va sin ser atendido.\n", id);
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    /* Hay silla libre: entra a la sala */
    pacientes_en_sala++;
    printf("Paciente %2d: se sienta en la sala.  En sala: %d\n",
           id, pacientes_en_sala);
    pthread_cond_signal(&hay_paciente); /* avisa al médico */
    pthread_mutex_unlock(&mutex);

    return NULL;
}

/* ------------------------------------------------------------------ */
/*  Hilo MÉDICO                                                         */
/* ------------------------------------------------------------------ */
void *funcion_medico(void *arg) {
    while (total_atendidos < p) {
        pthread_mutex_lock(&mutex);

        /* Si no hay nadie en sala, el médico duerme */
        while (pacientes_en_sala == 0 && total_atendidos < p) {
            printf("Medico   : sala vacia, esperando...\n");
            pthread_cond_wait(&hay_paciente, &mutex);
        }

        /* Comprobar condición de fin */
        if (total_atendidos >= p) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        /* Atender al siguiente paciente */
        pacientes_en_sala--;
        total_atendidos++;
        printf("Medico   : atendiendo paciente. En sala: %d  (atendidos: %d)\n",
               pacientes_en_sala, total_atendidos);
        pthread_mutex_unlock(&mutex);

        sem_post(&silla_libre); /* libera la silla que ocupaba el paciente */
        sleep(rand() % 2);      /* simula tiempo de consulta */
    }

    printf("Medico   : ha atendido a todos. Cerrando consulta.\n");
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  main                                                                */
/* ------------------------------------------------------------------ */
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <num_pacientes> <num_sillas>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    p = atoi(argv[1]); /* número total de pacientes */
    s = atoi(argv[2]); /* sillas en la sala de espera */

    /* Reserva dinámica de threads e ids (igual que en mudanza) */
    pthread_t  hilo_medico_t;
    pthread_t *hilos_pacientes = (pthread_t *)malloc(sizeof(pthread_t) * p);
    int       *ids             = (int *)malloc(sizeof(int) * p);

    /* Inicialización de mutex y condición */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&hay_paciente, NULL);

    /* Crear hilo médico */
    pthread_create(&hilo_medico_t, NULL, funcion_medico, NULL);

    /* Crear hilos paciente con pequeño retardo entre llegadas */
    for (int i = 0; i < p; i++) {
        ids[i] = i + 1;
        pthread_create(&hilos_pacientes[i], NULL, funcion_paciente, &ids[i]);
        usleep(200000); /* 200 ms entre llegadas */
    }

    /* Esperar a todos los pacientes */
    for (int i = 0; i < p; i++) {
        pthread_join(hilos_pacientes[i], NULL);
    }

    /* Despertar al médico por si quedó bloqueado esperando */
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&hay_paciente);
    pthread_mutex_unlock(&mutex);

    /* Esperar al médico */
    pthread_join(hilo_medico_t, NULL);

    /* Destrucción y liberación */
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&hay_paciente);

    free(hilos_pacientes);
    free(ids);

    return 0;
}
