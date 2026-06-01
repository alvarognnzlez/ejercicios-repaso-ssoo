#!/bin/bash

if test $# -gt 2 -o $# -lt 1
then
    echo "Uso: $0 ficheroEntrada [ficheroSalida]"
    exit 1
fi

if ! test -f "$1"
then
    echo "$1 no es un fichero valido"
    exit 1
fi

if test $# -eq 2
then
    salida="$2"
else 
    salida="default.txt"
fi

cut -d ";" -f 3 | sort -n > "$salida"


