#!/bin/bash
if test $# -gt 1  #comprobamos si nos pasan mas de un argumento
then 
    echo "Uso: \"$0 directorio\" o \"$0\" para buscar en el directorio actual"
    exit 1   

elif test $# -eq 1
then   
    if  test -d $1
    then
        directorio=$1
    else
    echo "No existe el directorio $1 o no es un directorio"
    exit 1
    fi
else 
    directorio="./"
fi

find $directorio -name "[ab]*" | grep -v '~'