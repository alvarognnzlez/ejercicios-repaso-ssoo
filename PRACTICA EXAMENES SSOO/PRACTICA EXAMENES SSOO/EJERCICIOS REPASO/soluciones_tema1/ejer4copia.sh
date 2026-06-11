#!/bin/bash

# ejer4copia.sh: lista los comandos de /bin que NO tienen página de manual

# No necesita argumentos
if test $# -ne 0
then
    echo "Uso: $0  (sin argumentos)"
    exit 1
fi

for i in $(ls /bin)   # ← el original usaba la variable $1 dentro del bucle en vez de $i
do
    if ! test -e "/usr/share/man/man1/$i.1.gz"   # ← era "$1" y "e /usr/..." (espacio extra)
    then
        echo "$i no tiene entrada de manual"
    fi
done

exit 0