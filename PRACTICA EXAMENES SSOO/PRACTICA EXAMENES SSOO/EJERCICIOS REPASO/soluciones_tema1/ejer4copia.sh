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

###################33


if test $# -ne 1
then
    echo "Uso incorrecto"
    echo "Uso $0"
    exit 1
fi

for i in ls`/bin`

do
    if ! test -e /usr/share/man/man1/{$1}.1.gz
    then    
        echo "El comando $1 no tiene pagina de manual"
        exit 0
    fi
done