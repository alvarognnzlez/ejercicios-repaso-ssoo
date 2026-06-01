#!/bin/bash
#comprobamos el numero de argumentos, no puede ser mayo que 1

if test $# -gt 1
    then
        echo "Uso \"$0 directorio \" o \"$0\" para buscar en el directorio actual"
        exit 1
    elif test $# -eq 1 #si se ha pasado un directorio, lo asignamos
    then 
    if ! test -d $1
    then
        echo "$1 no es un directorio valido o no es un directorio"
        exit 1
    else directorio=$1
    fi
    directorio="./"     
fi


for i in `find $directorio -name "*.sh" `
do
    chmod u+x $i
done


