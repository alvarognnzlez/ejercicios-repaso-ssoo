#!/bin/bash

if test $# -lt 1 || test $# -gt 2
then
    echo "Uso incorrecto"
    echo "Uso: $0 directorio1 [directorio2]"
    exit 1 
fi

if test $# -eq 1
then
    if ! test -d "$1" 
    then
        echo "Error, el directorio $1 no es un directorio valido"
        exit 1
    else
        directorio1="$1"
        directorio2="./"
    fi

else
    if ! test -d "$1" || ! test -d "$2"
    then
        echo "Error, directorios no validos"
        exit 1 
    else
        directorio1="$1"
        directorio2="$2"
    fi
fi

for i in $(find "$directorio1" -type f -name "*.mp3")
do
    cp "$i" "$directorio2" 
done