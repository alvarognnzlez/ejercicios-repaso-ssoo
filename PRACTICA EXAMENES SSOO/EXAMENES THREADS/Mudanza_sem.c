#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
int n,m,t1,t2;
int tandas;
int numTanda;
pthread_mutex_t mutex;
sem_t sem_descansillo;
sem_t sem_piso;
int cajasFurgo;
pthread_cond_t furgoLlena;


void* funcion_eq1(void* arg){ //el argumento que le voy a pasar es el id de la persona
    int id=*(int*)arg;

    while(1){
        sem_wait(&sem_piso); //restamos uno para q vaya haciendo m-1

        if(numTanda==tandas){ //repetimos este proceso hasta que el numero de tanda se igual al total de tandas
            pthread_exit(0);
        }
        printf("Equipo 1: persona %d Bajando al descansillo\n ",id);
        sleep(rand()%2);
        sem_post(&sem_descansillo):; //le mandamos el ok al semaforo del pasillo para que sepa que hay una caja para subir
    }
}

void* funcion_eq2(void* arg){
    int id=*(int*)arg;
    while(1){
        sem_wait(&sem_descansillo) //cogemos la caja que habia en el descansillo, y la vamos a subir a la furgo
        if(numTanda==tandas){ //repetimos este proceso hasta que el numero de tanda se igual al total de tandas
            pthread_exit(0);
        }
        printf("Equipo 2: persona %d Subiendo a la furgonet\n",id);
        sleep(rand()%2);
        pthread_mutex_lock(&mutex);
        cajasFurgo++;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&furgoLlena) //mandamos la señal para que compruebe si está llena la furgo        
    }

}


void* funcion_transportista(void* arg){
    while(numTanda<tandas){ //vamos a estar viajando hasta que se acaben las tandas
        pthread_mutex_lock(&mutex); //bloqueamos el mutex para comprobar el estado de la furgo, si esta llena o no
    while(cajasFurgo<m){//la furgo lleva m cajas
    pthread_cond_wait(&furgoLlena,&mutex);       
    }
    pthread_mutex_unlock(&mutex);

    printf("[Transportista] Realizando viaje");
    sleep(rand()%2);
    printf("[Transportista] Volviendo del viaje");
    sleep(rand()%2);
    //actualizamos aqui la tanda, cuando vuelve
    pthread_mutex_lock(&mutex);//bloqueamos mutex para actualizar variables
    numTanda++;
    cajasFurgo=0; //Ahora habra 0 cajas en la
    pthread_mutex_unlock(&mutex);
    //reactivamos el equipo 1, para que no se quede interbloqueado esperando permisos, tenemos que crear otra ronda m
    for(int i=0; i<m;i++){
        sem_post(&sem_piso);
    }

    }    
    //Si hemos salido del bucle significa que hemos completado las tandas, por lo que hay que liberar cada semaforo
    for(int i=0; i < t1 - m; i++){
        sem_post(&sem_piso);
    }
    for(int i=0; i < t2; i++){
        sem_post(&sem_descansillo);
    }

} 

int main(int argc, char** argv){

n=atoi(a0rgv[1]);
m=atoi(argv[2]);
t1=atoi(argv[3]);
t2=atoi(argv[4]);

//me creo un thread por cada tipo de proceso, subir bajar y transp.Tambien necesito un numero de id, y como no se cuantos hay, me los creo con memoria dinamica
//primero los declaro
pthread_t threadTranspor;
pthread_t* threads1 = (pthread_t*)malloc(sizeof(pthread_t)*t1);
pthread_t* threads2 = (pthread_t*)malloc(sizeof(pthread_t)*t2);
int* ids1 = (int*)malloc(sizeof(int)*t1);//multiplico por t1
int* ids2 = (int*)malloc(sizeof(int)*t2); //multiplico por t2



//me inicializo el mutex y los semaforos                        
pthread_mutex_init(&mutex;NULL);
sem_init(&sem_descansillo,0,0);
sem_init(&sem_piso,0,m);

//Luego los creo, como t1 y t2 nose cuantos son hago un bucle for, con la memoria dinamica reservada
pthread_create(&threadTranspor,NULL,funcion_transportista,NULL);

for(int i=0; i<t1; i++){
    ids1[i]=i;
    pthread_create(&threads1[i],NULL,funcion_eq1,&ids1[i]);
}
for(int i=0; i<t2; i++){
    ids2[i]=i;
    pthread_create(&threads2[i],NULL,funcion_eq2,&ids2[i]);
}

//Luego los elimino
for(int i=0; i<t1; i++){
    pthread_join(threads1[i],NULL);
}

for(int i=0; i<t2; i++){
    pthread_join(threads2[i],NULL);
}

pthread_join(threadTranspor,NULL);
pthread_mutex_destroy(&mutex);
sem_destroy(&sem_piso);
sem_destroy(&sem_descansillo);


free(threads1);
free(threads2);
free(ids1);
free(ids2);
}