/*
 * EJERCICIO: CONSULTORIO MÉDICO (versión PROCESOS)
 * =================================================
 *
 * Escenario:
 *   - Hay p pacientes y 1 médico.
 *   - La sala de espera tiene s sillas.
 *   - Los pacientes llegan uno a uno (con pequeño retardo).
 *   - Si hay silla libre, el paciente se sienta y envía SIGUSR1 al médico.
 *   - Si no hay silla, el paciente se va sin ser atendido.
 *   - El médico espera con pause(); al recibir SIGUSR1 atiende a un paciente.
 *   - Cuando todos los pacientes han terminado, el padre envía SIGTERM al médico.
 *
 * Llamadas usadas:
 *   fork, waitpid, kill, signal, pause, sleep, usleep,
 *   mmap (memoria compartida anónima para los contadores)
 *
 * Compilar: gcc -o consultorio_procesos consultorio_procesos.c
 * Ejecutar: ./consultorio_procesos <num_pacientes> <num_sillas>
 *           Ej: ./consultorio_procesos 8 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>   /* mmap */

/* ------------------------------------------------------------------ */
/* Manejador genérico: solo sirve para desbloquear pause()             */
/* ------------------------------------------------------------------ */
void manejador(int sig) { (void)sig; }

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <num_pacientes> <num_sillas>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int p = atoi(argv[1]);   /* número total de pacientes */
    int s = atoi(argv[2]);   /* sillas en la sala de espera */

    /* ------------------------------------------------------------------ */
    /* Memoria compartida: dos enteros visibles por todos los procesos.    */
    /*                                                                     */
    /* Con fork() normal, el hijo recibe una COPIA de la memoria del      */
    /* padre; los cambios no se ven entre sí. Con mmap MAP_SHARED los     */
    /* procesos sí comparten la misma región física.                       */
    /*                                                                     */
    /*   sillas_ocupadas : cuántas sillas están en uso ahora mismo        */
    /*   total_atendidos : cuántos pacientes ha atendido ya el médico      */
    /* ------------------------------------------------------------------ */
    int *sillas_ocupadas = mmap(NULL, sizeof(int),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *total_atendidos = mmap(NULL, sizeof(int),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (sillas_ocupadas == MAP_FAILED || total_atendidos == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    *sillas_ocupadas = 0;
    *total_atendidos = 0;

    /* ------------------------------------------------------------------ */
    /* El padre registra SIGUSR1 ANTES de crear al médico, porque el      */
    /* médico necesita el PID del padre para avisarle (getppid()).         */
    /* ------------------------------------------------------------------ */
    signal(SIGUSR1, manejador);

    /* ------------------------------------------------------------------ */
    /* Crear el proceso MÉDICO                                             */
    /* ------------------------------------------------------------------ */
    pid_t pid_medico = fork();
    if (pid_medico < 0) {
        perror("fork medico");
        exit(EXIT_FAILURE);
    }

    if (pid_medico == 0) {
        /* ────────────────────────────
         * CÓDIGO DEL MÉDICO
         * ──────────────────────────── */

        /* El médico reacciona a SIGUSR1 (paciente esperando)
         * y a SIGTERM (el padre le manda cerrar la consulta).          */
        signal(SIGUSR1, manejador);
        signal(SIGTERM, manejador);

        printf("Medico   : consulta abierta, esperando pacientes...\n");
        fflush(stdout);

        while (1) {
            /* Duerme hasta que llegue una señal */
            pause();

            /* Si ya no hay pacientes que atender, termina */
            if (*total_atendidos >= p)
                break;

            /* Atender al paciente que está esperando */
            (*sillas_ocupadas)--;
            (*total_atendidos)++;
            printf("Medico   : atendiendo paciente. "
                   "En sala: %d  (atendidos: %d/%d)\n",
                   *sillas_ocupadas, *total_atendidos, p);
            fflush(stdout);

            sleep(1);   /* simula tiempo de consulta */
        }

        printf("Medico   : ha atendido a todos. Cerrando consulta.\n");
        fflush(stdout);
        exit(0);
    }

    /* ------------------------------------------------------------------ */
    /* Crear los procesos PACIENTE con un pequeño retardo entre llegadas  */
    /* ------------------------------------------------------------------ */
    pid_t pids_pacientes[p];

    for (int i = 0; i < p; i++) {
        usleep(300000);   /* 300 ms entre llegadas para no saturar */

        pids_pacientes[i] = fork();
        if (pids_pacientes[i] < 0) {
            perror("fork paciente");
            exit(EXIT_FAILURE);
        }

        if (pids_pacientes[i] == 0) {
            /* ────────────────────────────
             * CÓDIGO DEL PACIENTE i+1
             * ──────────────────────────── */
            int id = i + 1;

            /* Comprobamos si hay silla libre.
             * NOTA: en un examen real habría que proteger esta sección
             * crítica con un semáforo. Aquí los retardos entre llegadas
             * minimizan la condición de carrera para simplificar.       */
            if (*sillas_ocupadas >= s) {
                printf("Paciente %2d: sala llena, se va sin ser atendido.\n", id);
                fflush(stdout);
                exit(0);
            }

            /* Hay silla: se sienta */
            (*sillas_ocupadas)++;
            printf("Paciente %2d: se sienta en la sala.  En sala: %d\n",
                   id, *sillas_ocupadas);
            fflush(stdout);

            /* Avisa al médico de que hay un paciente esperando */
            kill(pid_medico, SIGUSR1);

            exit(0);
        }
    }

    /* ------------------------------------------------------------------ */
    /* PROCESO PADRE: esperar a que terminen todos los pacientes          */
    /* ------------------------------------------------------------------ */
    for (int i = 0; i < p; i++) {
        waitpid(pids_pacientes[i], NULL, 0);
    }
    printf("Padre    : todos los pacientes han llegado.\n");
    fflush(stdout);

    /* Dar tiempo al médico a terminar con los que están en sala,
     * luego avisarle para que cierre (aunque ya haya terminado).        */
    sleep(p);   /* estimación generosa: p segundos de margen */

    /* Marcar que ya no habrá más pacientes y despertar al médico
     * por si quedó bloqueado en pause() con la sala vacía.              */
    *total_atendidos = p;
    kill(pid_medico, SIGTERM);

    /* Esperar al proceso médico */
    waitpid(pid_medico, NULL, 0);
    printf("Padre    : médico ha terminado. Fin de la simulación.\n");
    fflush(stdout);

    /* Liberar memoria compartida */
    munmap(sillas_ocupadas, sizeof(int));
    munmap(total_atendidos, sizeof(int));

    return 0;
}
