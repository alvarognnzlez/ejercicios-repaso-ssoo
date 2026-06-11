#!/bin/bash

# notasbcopia.sh: lee de ficheroEntrada y ordena numéricamente las notas (campo 3)

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

# ← era: cut -d ";" -f 3 | sort -n > "$salida"
# Faltaba el fichero de entrada en cut (sin él, cut espera stdin pero no hay pipe)
cut -d ";" -f 3 "$1" | sort -n > "$salida"
