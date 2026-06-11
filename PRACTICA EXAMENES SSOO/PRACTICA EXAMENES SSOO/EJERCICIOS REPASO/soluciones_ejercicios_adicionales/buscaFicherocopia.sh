#!/bin/bash

# Busca ficheros cuyo nombre sea como $1 seguido de un dígito y .txt
# Uso: $0 fichero [directorio]

if test $# -gt 2 -o $# -lt 1
then
    echo "Uso: $0 fichero [directorio]"
    exit 1
fi

if ! test -f "$1"
then
    echo "El archivo $1 no es un fichero valido"
    exit 1
fi

if test $# -eq 2
then
    directorio="$2"
else
    directorio="./"
fi

# ← era: find "$directorio" -type -f -name "{$1}[0-9].txt"
# Errores: "-type -f" (falta f separada), "{$1}" (las {} no expanden variables en -name)
find "$directorio" -type f -name "$1[0-9].txt"
