// necesitamos 2 procesos, uno que será el proceso padre y otro el proceso hijo,
// conectadas por 2 tuberías una irá direccion al hijo y otra el padre
// padre->escritura->hijo     hijo->lectura->padre
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


/*
int main (int argc, char** argv){

    int p_h[2];
    int h_p[2];
    pid_t pid;
    char buff[1024]


    pid = fork();

    if(pid==0){ //proceso hijo
        close(p_h[1]);
        close(h_p[0]);

        file* p_r = fdopen(p_h[0],'r');
        file* p_w = fdopen(h_p[1],'w');
        while(fgets(buffer,1024,p_r)!=NULL){
            // toUpper(buffer): converts the buffer string to uppercase
            fputs(buff,p_w);
            fflush(p_w);
        }
        fclose(p_r);
        fclose(p_w);
    }else{  //proceso padre
        close(h_p[0]);
        close(h_p[1]);

        file* p_r = fdopen(h_p[0],'r');
        file* p_w = fdopen(p_h[1],'w');

        while(fgets(buff,1024,stdin)!=NULL){
            fputs(buff,p_w);
            fflush(p_w);
            fgets(buff,1024,p_r);
            printf("%s\n",buff)
        }
    }
    fclose();
    fclose(p_w);
    wait(NULL);

    exit(0);

}

*/

int main(int argc, char **argv) {
  char buffer[1024];
  int pipe_padre[2]; // tuberia en la que escribe el padre
  int pipe_hijo[2];  // tubería en la que escribe el hijo
  pid_t pid;

  pipe(pipe_hijo);
  pipe(pipe_padre);

  pid = fork();

  if (pid > 0) { // error
    fprintf(stderr, "Error al hacer fork()", strerror(errno));
    exit(1);

  } else
}
