#!/bin/bash
#hay que buscar cuales de los ficheros en /bin NO tienen la pagina de manual
#tiene una pagina explicando el mandato en el manual.
#las páginas estan en /usr/share/man/man1/nombre.1.gz

#como tal no pide un numero de argumetnos simplemente tenemos ue buscar en una direccion

for i in `ls /bin`
do
    if ! test -e e /usr/share/man/man1/$1.1.gz
    then    
        echo "$1 no tiene entrada de manual"
    fi    
done

