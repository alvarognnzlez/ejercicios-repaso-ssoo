
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#define BUFFER_SIZE 10000

int buffer[10000];
pthread_mutex_t mutex;
int usar_mutex=0; //es una flag



void* manejador_escritor(void* arg){ //tiene que escribir 0,1,2,...10000
    int valor=0;
    while(1){
        if(usar_mutex){
            pthread_mutex_lock(&mutex);
        }

        for(int i=0; i<BUFFER_SIZE; i++){//escribo el mismo valor en las 10000 posiciones 
            buffer[i]=valor 
        }
        printf("Escritor, pasada %d",valor);
        if(usar_mutex){
            pthread_mutex_lock(&mutex);
        }
        valor++; 
    }

    pthread_exit(NULL);
}


void* manejador_lector(void* arg){
    while(1){
        if(usar_mutex){
            pthread_mutex_lock(&mutex);
        }
        int primero = buffer[0];
        int correcto=1;

        for(int i=0; i<BUFFER_SIZE; i++){
            if(buffer[i]!=primero){//compara cada posicion con la referencia
                
            correcto=0;
            break;
            } 
        }
    }

    if(correcto){
        printf("Lector CORRECTO: todos valen");
    }else{
        printf("Lector INCORRECTO: buffer incosistente");
    }

    if(usar_mutex){
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    if (argc==2){
        usar_mutex=1;
        printf("Ejecutando con mutex");
    }else{
        usar_mutex=0;
        printf("Ejecutando sin mutex");
    }

    pthread_mutex_init(&mutex,NULL);

    //creamos los threads.
    pthread_t escritor, lector;


    pthread_create(&manejador_escritor,NULL,escritor,NULL);

    pthread_create(&manejado_lector,NULL,lector,NULL);


    pthread_join(manejador_escrito,NULL);
    pthread_join(manejador_lector,NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}