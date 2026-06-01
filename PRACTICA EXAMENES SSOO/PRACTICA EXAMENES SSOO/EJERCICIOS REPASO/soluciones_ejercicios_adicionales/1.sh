#!/bin/bash

# a) Comprobar que el número de argumentos recibidos es al menos 2
if test $# -lt 2
then
    echo "Error: Número de argumentos insuficiente."
    echo "Uso: $0 <directorioOrigen> <directorioDestino1> [directorioDestino2] ..."
    exit 1
fi

# Comprobar que cada argumento es un directorio
for arg in "$@"
do
    if test ! -d "$arg"
    then
        echo "Error: '$arg' no existe o no es un directorio válido."
        exit 1
    fi
done

# b) Copiar los ficheros cuyo nombre cumple el formato especificado
# del directorio Origen a cada uno de los directorios Destino.

origen="$1"
shift # Desplazar los argumentos para que $@ contenga solo los destinos

# Buscamos los ficheros con el formato fic_<año>_<mes>_<día>.sec
# <año> es de 4 dígitos, <mes> de 2 dígitos y <día> de 2 dígitos.
# Usamos find con el patrón de caracteres [0-9] para asegurar la máxima portabilidad.
for fichero in $(find "$origen" -type f -name "fic_[0-9][0-9][0-9][0-9]_[0-9][0-9]_[0-9][0-9].sec")
do
    for destino in "$@"
    do
        cp "$fichero" "$destino"
        echo "Copiado: $fichero -> $destino"
    done
done
