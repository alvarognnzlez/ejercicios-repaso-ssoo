#!/bin/bash

# ejer2copia.sh: busca un fichero en todos los directorios del PATH

if test $# -ne 1
then
    echo "Uso: $0 archivo"
    exit 1
fi  # ← faltaba el fi que cerraba el if (el original tenía el cuerpo del script fuera del if)

NOMBRE_ARCHIVO="$1"
ENCONTRADO=0

echo "-Buscando archivo-----------------"

# ← era: for RUTA_PATH in `echo $PATH | tr -s ':' '\n'`
# Correcto: tr ':' '\n' (tr no necesita -s para esto, y la sintaxis de escape era incorrecta)
for RUTA_PATH in $(echo "$PATH" | tr ':' '\n')
do
    # ← era: CADENA=`find $RUTA_PATH -name $NOMBRE_ARCHIVO` (sin comillas, vulnerable a espacios)
    CADENA=$(find "$RUTA_PATH" -name "$NOMBRE_ARCHIVO" 2>/dev/null)
    if test -n "$CADENA"   # ← era: ! test -z (equivalente pero menos claro)
    then
        echo "$NOMBRE_ARCHIVO está en $CADENA"
        ENCONTRADO=1
    fi
done

if test $ENCONTRADO -eq 0
then
    echo "$NOMBRE_ARCHIVO no se ha encontrado en la ruta del PATH"  # ← typo $NOMBREARCHIVO → $NOMBRE_ARCHIVO
    exit 1
fi

exit 0