#!/bin/bash

# -------------------------------------------------------
# filtra_archivos.sh
# Uso: filtra_archivos.sh <ext1> <ext2> <ext3> <fichSal> [ruta]
# -------------------------------------------------------

# a) Comprobación de argumentos

if test $# -lt 4 -o $# -gt 5
then
    echo "Numero de argumentos incorrectos" >&2
    echo "Uso: $0 <ext1> <ext2> <ext3> <fichSal> [ruta]" >&2
    exit 1
elif test $# -eq 5 -a ! -d "$5"
then
    echo "$5 no existe o no es un directorio" >&2
    exit 1
elif test $# -eq 5
then
    ruta="$5"
else
    ruta="./"
fi

ext1="$1"
ext2="$2"
ext3="$3"
fichSal="$4"

# -------------------------------------------------------
# b) Funcionalidad
# -------------------------------------------------------

total_archivos=0
total_bytes=0

# Escribir/añadir cabecera al fichero de salida
{
    echo "Directorio analizado: $ruta"
    echo "Fecha de ejecución: $(date)"
    echo "Extensión | # de archivos | Tamaño total"
} >> "$fichSal"

# Procesar cada extensión
for ext in "$ext1" "$ext2" "$ext3"; do

    num_archivos=0
    suma_bytes=0

    # Buscar recursivamente los ficheros con esa extensión
    while IFS= read -r fichero; do
        num_archivos=$(( num_archivos + 1 ))
        bytes=$(stat -c %s "$fichero")
        suma_bytes=$(( suma_bytes + bytes ))
    done < <(find "$ruta" -type f -name "*.$ext")

    # Añadir línea de esta extensión al fichero de salida
    echo "$ext       | $num_archivos           | $suma_bytes bytes" >> "$fichSal"

    total_archivos=$(( total_archivos + num_archivos ))
    total_bytes=$(( total_bytes + suma_bytes ))
done

# Escribir totales
{
    echo "Total archivos: $total_archivos"
    echo "Total tamaño: $total_bytes bytes"
} >> "$fichSal"

exit 0
