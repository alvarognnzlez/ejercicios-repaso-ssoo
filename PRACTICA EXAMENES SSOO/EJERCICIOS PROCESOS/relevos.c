/*
 * EJERCICIO: CARRERA DE RELEVOS
 * ==============================
 * El padre crea 4 hijos. Los hijos esperan una señal (SIGUSR1) para empezar
 * a "correr". Cada hijo al terminar pasa el testigo (señal) al siguiente
 * hermano. El último hijo avisa al padre con SIGUSR2.
 *
 * Llamadas usadas: fork, signal, kill, pause, sleep, wait, getpid
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 4 /* Número de hijos (corredores) */

/* Manejador vacío: solo sirve para desbloquear pause() */
void manejador(int sig) {}

int main(void) {
  pid_t pids[N]; // guardamos los 4 pids de los hijos aquí.

  /* El padre registra su manejador para SIGUSR2 ANTES de crear hijos */
  signal(SIGUSR2,
         manejador); // El padre registra su manejador antes de crear hijos.

  /* ── Crear los 4 hijos ── */
  for (int i = 0; i < N; i++) {
    pids[i] = fork(); // En cada iteración se crea un hijo. fork() devuelve:Al
                      // padre → el PID del hijo recién creado (se guarda en
                      // pids[i]) Al hijo → 0

    if (pids[i] < 0) {
      perror("fork");
      exit(1);
    }

    if (pids[i] == 0) {
      /* ─────────────────────────────
       * CÓDIGO DEL HIJO i
       * ───────────────────────────── */

      /* Registrar manejador para recibir el testigo */
      signal(SIGUSR1, manejador);

      /* Esperar a que nos pasen el testigo */
      pause();

      /* Correr */
      printf("Soy el proceso %d, corriendo . . .\n", getpid());
      fflush(stdout);
      sleep(2);

      /* Pasar el testigo */
      if (i < N - 1) {
        printf("Terminé. Paso el testigo al proceso %d\n", pids[i + 1]);
        fflush(stdout);
        kill(pids[i + 1], SIGUSR1);
      } else {
        printf("Terminé!\n");
        fflush(stdout);
        kill(getppid(), SIGUSR2); /* Último hijo avisa al padre */
      }

      exit(0);
    }
  }

  /* ─────────────────────────────
   * CÓDIGO DEL PADRE
   * ───────────────────────────── */

  printf("Todos los hijos creados.\n");
  fflush(stdout);

  /* Esperar a que todos los hijos estén bloqueados en pause() */
  sleep(1);

  /* Dar la salida al primer hijo */
  printf("Doy la salida!\n");
  fflush(stdout);
  kill(pids[0], SIGUSR1);

  /* Esperar a que el último hijo avise con SIGUSR2 */
  pause();

  /* Recoger a todos los hijos (evitar zombies) */
  for (i = 0; i < N; i++) {
    wait(NULL);
  }

  printf("Todos los hijos han terminado.\n");
  return 0;
}
