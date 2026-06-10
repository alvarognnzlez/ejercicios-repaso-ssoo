#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
int N; //NUMERO DE usuarios
sem_t sem_sala; //3 puestos de preparación
pthread_mutex_t mutex;
pthread_cond_t cond_usuarios; //en esta condicion los usuarios esperan para imprimir
pthread_cond_t cond_tecnico //el tecnico espera en esta condicion

int impresiones=0; //maximo de 5 impresiones hasta que haya un mantenimiento

int manteniemto_pendiente=0; //esto es un binario para saber si el mantenimiento esta pendiente
int impresora_libre=1; //la impresora al principio esta libre


void* manejador_usuarios(void*arg){
    int id = *(int*) arg
    while(1){
        //lo primero que hacemos es coger un puesto d impresion, maximo hay 3
        sem_wait(&sem_sala) //con esto restamos 1
        printf("Usuario %d: Preparando documento\n",id); //preparamos el documento
        sleep(rand()%3);
        sem_post(&sem_sala) //liberamos la sala que hemos usado para preparar.
        //ahora vamos a intentar imprimir, tiene que cumplirse 2 condiciones, que la impresora no este usandose
        //  y que no este en mantenimiento

        //esperamos que no haya mantenimiento y que la impresora este libre
        pthread_mutex_lock(&mutex);
        while(mantemiento_pendiente || !impresora_libre){
            pthread_cond_wait(&cond_usuarios,&mutex);
        }

        //cuando no ocurra ninguna de estas, entonces reservamos la impresora para usarla nosotros;
        impresora_libre=0;
        pthread_mutex_unlock(&mutex);

        //imprimimos
        printf("Usuario %d. Imprimiendo documento...\n", id);
        sleep(rand()%2);

        //indicamos que hemos impreso, liberamos la impresora y sumamos una impresion, y si llevamos 5 impresiones, avisamos al tecnico
        pthread_mutex_lock(&mutex);
        impresiones++;
        printf("Impresión realizada\n");
        impresora_libre=1;


        if(impresiones % 5==0){
            mantenimiento_pendiente=1;
            printf("Mantenimiento necesario.\n")
            pthread_cond_signal(&cond_mantenimiento);
        }else{
            //Como no hay mantenimiento necesario, despertamos un usuario en cadena
            pthread_cond_signal(&cond_usuarios);
        }
        pthread_mutex_lock(&mutex);


    }
    pthread_exit(NULL);
}


void* manejador_tecnico(){
    while(1){
        pthread_mutex_lock(&mutex);
        //si hay mantenimiento ningun usuario puede imprimir.
        while(!mantenimiento_pendiente){ //tenemos que esperar a q sea necesario mantenimiento
            pthread_cond_wait(&cond_tecnico,&mutex);
        }
        impresora_libre=0; //reservamo la impresora para hacer el mantenimiento
        pthread_mutex_unlock(&mutex);
        printf("Realizando mantenimiento");
        sleep(rand()%2);
        printf("Mantenimiento Realizado");

        //restablecemos todo y despertamos a un usuario;

        pthread_mutex_lock(&mutex);
        mantenimiento_pendiente=0;
        impresora_libre=1;
        pthread_cond_signal(&cond_usuario,&mutex);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    if(argc!=2){
        fprintf(stderr,"Uso: %s N_Users\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    N=atoi(argv[1]);
    if(N<=0){
        fprintf(stderr,"Error, N debe ser mayor que 0");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    //inicializamos primero siempre los semaforos
    sem_init(&sem_sala,0,3);
    //ahora los mutex y condiciones
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond_usuarios);
    pthread_cond_init(&cond_tecnico);

    //reservamos memoria para N usuarios
    pthread_t tecnico;
    pthread_t* usuarios = (pthread_t*)malloc(sizeof(pthread_t)*N);
    int* ids = (int*)malloc(sizeof(int)*N);

    //ahora creamos los hilos
    pthread_create(&tecnico,NULL,manejador_tecnico,NULL);

    for(int i=0; i<N;i++){
        ids[i]=i;
        pthread_create(&usuarios[i],NULL,manejador_usuarios,&ids[i]);
    }

    pthread_join(tecnico,NULL);

    for(int i=0; i<N;i++){
        pthread_join(usuarios,NULL);
    }

    sem_destroy(&sem_sala);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_usuarios);
    pthread_cond_destroy(&cond_tecnico);


    free(usuarios);
    free(ids);
    return 0;
}