#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
  pid_t pid; // este será el proceso
  int status;

  if (argc < 2) {
    printf("Uso: %s programa argumentos \n", argv[0]);
    exit 0:
  }
}