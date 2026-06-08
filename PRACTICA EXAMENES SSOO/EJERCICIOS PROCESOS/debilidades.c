/*
 * debilidades.c
 *
 * Uso: ./debilidades num_lineas_cabecera fichero_salida
 *
 * El programa crea DOS procesos hijos que colaboran con el padre:
 *
 *   PADRE   --> (pipe1) --> HIJO1 (blueprint_analysis) --> fichero_salida
 *   PADRE   --> (pipe2) --> HIJO2 (muestra cabecera y comprueba existencia)
 *
 * El padre lee nombres de fichero de stdin y los envía a ambos hijos.
 * HIJO1: redirige su stdin al extremo de lectura de pipe1 y ejecuta
 *        blueprint_analysis, redirigiendo su stdout al fichero_salida.
 * HIJO2: lee nombres de pipe2, comprueba que el fichero existe y muestra
 *        las primeras num_lineas_cabecera líneas por stdout.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char *argv[]) {

  /* ------------------------------------------------------------------ */
  /* d) Comprobación de argumentos y apertura del fichero de salida       */
  /* ------------------------------------------------------------------ */
  if (argc != 3) {
    fprintf(stderr, "Uso: %s num_lineas_cabecera fichero_salida\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int num_lineas = atoi(argv[1]);
  if (num_lineas <= 0) {
    fprintf(stderr, "Error: num_lineas_cabecera debe ser un entero positivo\n");
    exit(EXIT_FAILURE);
  }

  /* Abrimos (o creamos) el fichero de salida en modo escritura */
  int fd_salida = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd_salida == -1) {
    fprintf(stderr, "Error al abrir el fichero de salida '%s': %s\n", argv[2],
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* ------------------------------------------------------------------ */
  /* e) Creación de pipes y procesos                                      */
  /* ------------------------------------------------------------------ */

  /* pipe1: padre --> hijo1 (blueprint_analysis)  */
  int pipe1[2];
  /* pipe2: padre --> hijo2 (cabecera)            */
  int pipe2[2];

  if (pipe(pipe1) == -1) {
    perror("pipe1");
    exit(EXIT_FAILURE);
  }
  if (pipe(pipe2) == -1) {
    perror("pipe2");
    exit(EXIT_FAILURE);
  }

  /* ---- FORK HIJO 1: ejecuta blueprint_analysis ---- */
  pid_t pid1 = fork();
  if (pid1 == -1) {
    perror("fork hijo1");
    exit(EXIT_FAILURE);
  }

  if (pid1 == 0) {
    /* HIJO 1 */
    /* Cerramos extremo de escritura del pipe1 (solo leemos) */
    close(pipe1[1]);
    /* No usamos pipe2 */
    close(pipe2[0]);
    close(pipe2[1]);

    /* Redirigimos stdin al extremo de lectura de pipe1 */
    if (dup2(pipe1[0], STDIN_FILENO) == -1) {
      perror("dup2 stdin hijo1");
      exit(EXIT_FAILURE);
    }
    close(pipe1[0]);

    /* Redirigimos stdout al fichero de salida */
    if (dup2(fd_salida, STDOUT_FILENO) == -1) {
      perror("dup2 stdout hijo1");
      exit(EXIT_FAILURE);
    }
    close(fd_salida);

    /* Ejecutamos blueprint_analysis (debe estar en PATH o en
     * ./blueprint_analysis) */
    execlp("./blueprint_analysis", "blueprint_analysis", NULL);

    /* Si llegamos aquí, exec ha fallado */
    perror("execlp blueprint_analysis");
    exit(EXIT_FAILURE);
  }

  /* ---- FORK HIJO 2: muestra cabecera y comprueba existencia ---- */
  pid_t pid2 = fork();
  if (pid2 == -1) {
    perror("fork hijo2");
    exit(EXIT_FAILURE);
  }

  if (pid2 == 0) {
    /* HIJO 2 */
    /* Cerramos extremo de escritura de pipe2 (solo leemos) */
    close(pipe2[1]);
    /* No usamos pipe1 */
    close(pipe1[0]);
    close(pipe1[1]);
    close(fd_salida);

    /* Abrimos el extremo de lectura de pipe2 como FILE* para usar fgets */
    FILE *fp_pipe = fdopen(pipe2[0], "r");
    if (fp_pipe == NULL) {
      perror("fdopen pipe2 hijo2");
      exit(EXIT_FAILURE);
    }

    char filename[BUFSIZE];

    /* Leemos nombres de fichero del pipe2 enviados por el padre */
    while (fgets(filename, BUFSIZE, fp_pipe) != NULL) {

      /* Eliminar '\n' final si lo hay */
      if (filename[strlen(filename) - 1] == '\n') {
        filename[strlen(filename) - 1] = '\0';
      }

      /* Comprobar que el fichero existe */
      FILE *fp_plano = fopen(filename, "r");
      if (fp_plano == NULL) {
        fprintf(stderr,
                "Error: el fichero '%s' no existe o no se puede abrir\n",
                filename);
        continue;
      }

      /* Mostrar las primeras num_lineas líneas (cabecera) por stdout */
      char linea[BUFSIZE];
      int count = 0;
      while (count < num_lineas && fgets(linea, BUFSIZE, fp_plano) != NULL) {
        printf("%s", linea);
        count++;
      }
      fflush(stdout);

      fclose(fp_plano);
    }

    fclose(fp_pipe);
    exit(EXIT_SUCCESS);
  }

  /* ------------------------------------------------------------------ */
  /* PROCESO PADRE                                                        */
  /* ------------------------------------------------------------------ */

  /* El padre solo escribe en los pipes; cerramos extremos de lectura */
  close(pipe1[0]);
  close(pipe2[0]);
  close(fd_salida); /* El padre no necesita el fd del fichero de salida */

  /* Abrimos los extremos de escritura como FILE* para usar fputs cómodamente */
  FILE *fp_pipe1 = fdopen(pipe1[1], "w");
  FILE *fp_pipe2 = fdopen(pipe2[1], "w");

  if (fp_pipe1 == NULL || fp_pipe2 == NULL) {
    perror("fdopen padre");
    exit(EXIT_FAILURE);
  }

  char filename[BUFSIZE];

  /* Leemos nombres de fichero de stdin y los enviamos a ambos hijos */
  while (fgets(filename, BUFSIZE, stdin) != NULL) {

    /* Eliminar '\n' final si lo hay */
    if (filename[strlen(filename) - 1] == '\n') {
      filename[strlen(filename) - 1] = '\0';
    }

    /* Enviamos al hijo1 (blueprint_analysis) con '\n' para separar nombres */
    fprintf(fp_pipe1, "%s\n", filename);
    fflush(fp_pipe1);

    /* Enviamos al hijo2 (cabecera) con '\n' */
    fprintf(fp_pipe2, "%s\n", filename);
    fflush(fp_pipe2);
  }

  /* Cerramos los pipes para señalar EOF a los hijos */
  fclose(fp_pipe1);
  fclose(fp_pipe2);

  /* Esperamos a que ambos hijos terminen */
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  exit(EXIT_SUCCESS);
}
