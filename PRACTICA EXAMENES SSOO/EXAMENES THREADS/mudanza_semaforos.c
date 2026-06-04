#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int n,m,t1,t2;
sem_t sem_piso,sem_descansillo;
pthread_mutex_t mutex;
//me tengo que crear una variable para contar el numero de cajas que hay en la furgo
int cajasFurgo;
//tambien necesito una condicion para el mutex, que indique si la furgo esta llena
pthread_cond_t furgoLlena;

int tanda, nTandas;

void* funcion_eq1(void* arg){
    int id = *(int*) arg;
    while(1){
        sem_wait(&sem_piso); //resto -1 a m para saber que estoy bajando una caja
         if(tanda==nTandas){
            pthread_exit(0); //el thread se ejecuta hasta que no queden mas tandas que hacer
        }

        printf("Equipo 1: persona %d->Bajando al descansillo");
        sleep(rand() %2);
        sem_post(&sem_descansillo); //sumo 1 al descansillo para dar a entender que hay una caja en el descansillo y se puede subir a la furgo
    }
}
void* funcion_eq2(void* arg){
    int id = *(int*) arg; //asigno el id de la persona que esta trabajando
    while(1){
        sem_wait(&sem_descansillo);
        if(tanda==nTandas){
            pthread_exit(0); //el thread se ejecuta hasta que no queden mas tandas que hacer
        }



        printf("Equipo 2: persona %d -> Subiendo a la furgoneta");
        sleep(rand()%2);
        pthread_mutex_lock(&mutex); //ahora vamos a cerrar el mutex pq vamos a usar y modificar cajasFurgo
        cajasFurgo++; //al subir la caja a la furgo, aumento la variable
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&furgoLlena);  //enviamos una señal a la condicion del mutex para que compruebe si la furgo esta llena, con el valor modificado de cajasFurgo
    }
    
}
void* funcion_transportista(void* arg){
    //lo primero es comprobar que no se han cumplido todas las tandas;

    while(tanda<nTandas){
        pthread_mutex_lock(&mutex); //bloqueamos el mutex para comprobar el valor de "cajasFurgo"
        while (cajasFurgo<m){
            pthread_cond_wait(&furgoLlena, &mutex); //aqui tenemos que darle la señal al mutex de que todavia el numero de cajas en la furgo es menor que el total de cajas, por lo que nom se puede ir
        }
        //fuera del while de cajasFurgo significa que esta ya llena la furgo, hacemos el trabajo del transportista

        printf("Transportista: Realizando viaje\n");
        sleep(rand()%2);
        printf("Transportista: Volviendo del viaje\n");
        //actualizamos ahora la tanda por la que vamos, como se encarga unicamente la funcion del transportista de actualizar el numero de tanda, no es un recurso compartido y no necesitamos hacer un mutex ni nada        
        tanda++;
        cajasFurgo=0; //ahora vaciamos las cajas de la furgo, aqui no necesitamos el mutex pq los semaforos confirmarn que el eq2 no va a modificar el valor de cajasFurgo pq no hay mas cajas en el descansillo para subir.
        //ahora reiniciamos el numero de cajas en el piso, para poner a trabajar al eq1 de nuevo
        for (int i = 0; i < m; i++){
            sem_post(&sem_piso); //sem_post suma x cajas, que luego con sem_Wait irá reduciendo de nuevo hasta bajar todas las cajas al descansillo
        }        
    }

    //ahora abrimos todos los semaforos para que acaben todos los threads.Cuando el transportista completa la última tanda (tanda == nTandas), todos los hilos están bloqueados esperando en sus semáforos. Hay que desbloquearlos para que puedan comprobar la condición de salida y hacer pthread_exit.
    for (int i = 0; i < t1 - m; i++) sem_post(&sem_piso);
    for (int i = 0; i < t2; i++) sem_post(&sem_descansillo);

    pthread_exit(0);


}
int main(int argc, char** argv){
    n=atoi(argv[1]);
    m=atoi(argv[2]);
    t1=atoi(argv[3]);
    t2=atoi(argv[4]);
    nTandas=n/m;
    tanda=0;

    //pasamos los argumentos a variables, y ahora creamos cada thread necesario (1 por persona del cada equipo)
    pthread_t threadTranspor;  //este thread tendrá un mismo tamaño siempre
    pthread_t* threads1 = (pthread_t *)malloc(sizeof(pthread_t)*t1); //como habrá t1 personas, reservamos memoria dinamica pq no sabemos cuantos threads necesitaremos ni su tamaño
    pthread_t* threads2 = (pthread_t *)malloc(sizeof(pthread_t)*t2);  //como habrá t1 personas, reservamos memoria dinamica pq no sabemos cuantos threads necesitaremos ni su tamaño
    int* ids1 = (int *)malloc(sizeof(int) * t1);
    int* ids2 = (int *)malloc(sizeof(int) * t2);


    //voy a necesitar 2 semaforos, uno para el descansillo y otro para el piso

    sem_init(&sem_piso,0,m); //controla cuántos muebles/cajas hay disponibles en el piso para sacar. al iniciar estan las m cajas en el piso
    sem_init(&sem_descansillo,0,0); //controla cuántos objetos hay en el descansillo esperando ser recogidos. al iniciar hay 0 cajas abajo
    pthread_mutex_init(&mutex,NULL);


    for(int i=0; i<t1;i++){
            ids1[i]=i;
            pthread_create(&threads1[i],NULL,funcion_eq1,&ids1[i]);
    }
    for(int i=0; i<t2;i++){
            ids2[i]=i;
            pthread_create(&threads2[i],NULL,funcion_eq2,&ids2[i]);
    }

    pthread_create(&threadTranspor,NULL,funcion_transportista,NULL);


    for(int i=0; i<t1;i++){
    
            pthread_join(threads1[i],NULL);
    }

    for(int i=0; i<t2;i++){
    
            pthread_join(threads2[i],NULL);
    }

    pthread_join(threadTranspor,NULL);

    pthread_mutex_destroy(&mutex);
    sem_close(&sem_descansillo);
    sem_close(&sem_piso);
    pthread_cond_destroy(&furgoLlena);



    free(threads1);
    free(threads2);
    free(ids1);
    free(ids2);
    return 0;



}