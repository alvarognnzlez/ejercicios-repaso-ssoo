#!/bin/bash

if test $# -lt 4 -o $# -gt 5  # ← era "-o -gt 5" (faltaba $#)
then
    echo "Uso $0 <ext1> <ext2> <ext3> <fichSal> [<ruta a buscar>]"
    exit 1

elif test $# -eq 5 -a ! -d "$5"  # ← añadidas comillas a $5
then
    echo "$5 no es una ruta valida"
    exit 1
elif test $# -eq 5
then
    ruta="$5"
else
    ruta="./"
fi

# almaceno cada extensión y el fichero de salida
ext1="$1"
ext2="$2"
ext3="$3"
fichSal="$4"

total_archivos=0
total_bytes=0

{
    echo "Directorio analizado: $ruta"
    echo "Fecha de ejecución: $(date)"
    echo "Extensión | # de archivos | Tamaño total"
} >> "$fichSal"

# procesamos cada extensión del directorio
for ext in "$ext1" "$ext2" "$ext3"  # ← variable renombrada a ext (era i pero luego usaba $ext)
do
    archivos_ext=0
    bytes_ext=0

    while read -r fichero   # ← era "while read -r fichero do" (do no va aquí)
    do
        archivos_ext=$((archivos_ext + 1))
        bytes=$(stat -c %s "$fichero")  # ← era $((stat ...)) — $() no $(()); stat no es aritmética
        bytes_ext=$((bytes_ext + bytes))
    done < <(find "$ruta" -type f -name "*.$ext")  # ← era "$i" pero la variable del for ahora es $ext

    echo "$ext       | $archivos_ext           | $bytes_ext bytes" >> "$fichSal"
    total_archivos=$((total_archivos + archivos_ext))
    total_bytes=$((total_bytes + bytes_ext))
done

{
    echo "Total archivos: $total_archivos"
    echo "Total bytes: $total_bytes"
} >> "$fichSal"

exit 0
