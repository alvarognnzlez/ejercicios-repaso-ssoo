#!/bin/bash

#admite uno o varios argumentos
#filesize.sh directorio [fichero1] [fichero2] [...]
#    [0]            [1]       [2]



if test $# -lt 1
    then echo "Uso: $0 directorio fichero1 fichero2 ..."
    exit 1

    elif test $# -eq 1 #solo le pasamos un directorio, sin fichero
    then 
        if test -d $1
            then echo "No existen ficheros"
            exit 0
        else
            then echo "$1 no es un directorio valido"
            exit 1
        fi
    else #aqui nos han pasado mas de 1 argumento
        if test -d $1
        then 
            directorio=$1
            shift #almaceno el $1 en directorio y luego desplazo los argumentos a la izq, ahora $1 es el fichero1
        else
            echo "$1 no es un directorio valido"
            exit 1
        fi    
fi

for fichero in $@
do
    if test -e $fichero 
    then
        tamano=`du -b $fichero | cut -f1`
        echo "$fichero $tamano bytes"
    elif test -e "$directorio/$fichero"
        then tamano=`du -b $fichero | cut -f1`
        echo "$fichero $tamano bytes"
    else
        echo "No se ha encontrado el fichero $fichro en ninguna ruta"
    fi
done


#ahora tratamos los ficheros de los argumentos.

