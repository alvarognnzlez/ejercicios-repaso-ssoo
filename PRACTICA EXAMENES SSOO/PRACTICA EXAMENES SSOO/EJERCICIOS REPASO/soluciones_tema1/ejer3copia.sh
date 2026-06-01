#!/bin/bash 


#tiene que recibir un argumento, y buscar los dos 
# archivos de mas tamaño y muestre nombre y tamaño

if test $# -gt 1 #necesita como mucho un argumento, no mas
    then
        echo -e "Uso: \"$0 directorio\" o  \"$0\" para buscar en el directorio actual"
        exit 1
elif test $# -eq 1 #comprobamos si hemos pasado algun directorio
    then 
        if test -d $1 #comprobamos que el directorio es un directorio valido
        then
           directorio=$1
        else
            echo "$1 no es un directorio valido o no es un directorio"
            exit 1
            
        fi
    else #no hemos pasado ningun directorio, usamos el actual
    directorio="./"
fi

cd $directorio
lista = `ls` #creamos una lista donde almacenaremos todos los valores obtenidos con ls
du -b lista | sort -n -r | head -2
