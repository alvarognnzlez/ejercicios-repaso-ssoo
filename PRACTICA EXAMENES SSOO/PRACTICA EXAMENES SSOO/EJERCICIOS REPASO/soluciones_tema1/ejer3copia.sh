#!/bin/bash

# ejer3copia.sh: muestra los 2 ficheros más grandes del directorio dado

if test $# -gt 1
then
    echo "Uso: \"$0 directorio\" o \"$0\" para buscar en el directorio actual"
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
fi

# ← el original hacía:
#   lista = `ls`          → espacios alrededor de = (error de sintaxis)
#   du -b lista           → "lista" es una variable sin $, se trata como nombre de fichero
#
# Corrección: usar find + du directamente, ordenar y mostrar los 2 mayores
find "$directorio" -type f -exec du -b {} \; | sort -rn | head -2