#!/bin/bash

# ejer5copia.sh: da permisos de ejecución a todos los .sh del directorio dado

if test $# -gt 1
then
    echo "Uso \"$0 directorio\" o \"$0\" para buscar en el directorio actual"
    exit 1
elif test $# -eq 1
then
    if ! test -d "$1"
    then
        echo "$1 no es un directorio valido"
        exit 1
    fi
    directorio="$1"
else
    directorio="./"
    # ← el original asignaba directorio="$1" en el else y luego lo sobreescribía con "./"
    # lo cual hacía que nunca se usara el directorio pasado como argumento
fi

for i in $(find "$directorio" -name "*.sh")
do
    chmod u+x "$i"   # ← añadidas comillas a $i
done
