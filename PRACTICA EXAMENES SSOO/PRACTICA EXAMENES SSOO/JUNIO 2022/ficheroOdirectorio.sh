#!/bin/bash

# a) Comprobación del número de argumentos (1 o 2)
if test $# -lt 1 || test $# -gt 2
then
    echo "Uso incorrecto" >&2
    echo "Uso correcto: $0 palabra [directorio/fichero]" >&2
    exit 1
fi

palabra="$1"

# Comprobación del tipo de argumentos
if test $# -eq 2
then
    if test -f "$2"
    then
        modo="fichero"
        destino="$2"
    elif test -d "$2"
    then
        modo="directorio"
        destino="$2"
    else
        echo "Error: '$2' no es un fichero ni un directorio valido" >&2
        exit 1
    fi
else
    # Si no se introduce el segundo argumento, se usa el directorio actual
    modo="directorio"
    destino="./"
fi

# b) Si el segundo parámetro es un fichero
if [ "$modo" = "fichero" ]
then
    # Busca las líneas que contienen la palabra y cuenta el número de ocurrencias
    grep -c "$palabra" "$destino"
fi

# c) Si el segundo parámetro es un directorio o no se introdujo
if [ "$modo" = "directorio" ]
then
    # Busca de forma recursiva ficheros regulares mayores de 1024 bytes
    # cuyo nombre empiece por la palabra del primer parámetro.
    # Se listan en formato largo con 'ls -l'.
    find "$destino" -type f -size +1k -name "$palabra*" -exec ls -l {} \;
fi

exit 0
