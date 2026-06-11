#!/bin/bash

# filesize.sh directorio [fichero1] [fichero2] [...]
# Muestra el tamaño de los ficheros dados, buscándolos también dentro del directorio

if test $# -lt 1
then
    echo "Uso: $0 directorio fichero1 fichero2 ..."
    exit 1

elif test $# -eq 1
then
    # Solo han pasado el directorio, sin ficheros
    if ! test -d "$1"   # ← la lógica estaba al revés: era "if test -d" → "echo No existen ficheros"
    then
        echo "$1 no es un directorio valido"
        exit 1
    else
        echo "No se han proporcionado ficheros para buscar"
        exit 0
    fi

else
    # Más de 1 argumento: primero comprobamos que $1 es directorio
    if ! test -d "$1"
    then
        echo "$1 no es un directorio valido"
        exit 1
    fi
    directorio="$1"
    shift   # ahora $@ son solo los ficheros
fi

for fichero in "$@"
do
    if test -e "$fichero"
    then
        tamano=$(du -b "$fichero" | cut -f1)   # ← backticks → $() más moderno; añadidas comillas
        echo "$fichero $tamano bytes"
    elif test -e "$directorio/$fichero"
    then
        tamano=$(du -b "$directorio/$fichero" | cut -f1)  # ← era "du -b $fichero" (buscaba en ruta incorrecta)
        echo "$directorio/$fichero $tamano bytes"
    else
        echo "No se ha encontrado el fichero $fichero en ninguna ruta"  # ← typo "$fichro" → "$fichero"
    fi
done
