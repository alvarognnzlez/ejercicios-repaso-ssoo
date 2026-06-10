
/*
10 empleados,3 puestos de preparación, 1 impresora compartida
Tecnico hace mantenimiento cada 5 impresiones con prioridad
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


#define N_USERS 10
#define N_PUESTOS 3
#define MANT_CADA 5

int main() { pthread_t usuarios }