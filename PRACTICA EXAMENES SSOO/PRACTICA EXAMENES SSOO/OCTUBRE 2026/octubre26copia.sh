#!/bin/bash


if test $# -lt 4 -o -gt 5
    then 
        echo "Uso $0 <ext1> <ext2> <ext3> <fichSal> [<ruta a buscar>]"
        exit 1 

    elif test $# -eq 5 -a ! -d $5 
    then 
        echo "$5 no es una ruta valida"
        exit 1
    elif test $# -eq 5 #si hay 5 argumentos exactos y es un directorio valido
    then
        ruta="$5"
    else #hay solo 4 argumentos
    ruta="./"
fi


#me almaceno cada extensión y el fichero de salida
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
}>> "$fichSal"

#ahora tenemos que procesar cada extensión del directorio

for i in "$ext1" "$ext2" "$ext3"

do
archivos_ext=0
bytes_ext=0

while read -r fichero do
    archivos_ext=$((archivos_ext+1))  #sumamos 1 al numero total de archivos analizados
    bytes=$((stat -c %s "$fichero" )) #obtenemos los bytes del fichero que estamos analizando
    bytes_ext=$((bytes_ext + bytes))
    done< <(find "$ruta" -type f -name "*.$ext")

    echo " "$ext"       | "$archivos_ext"       | "$bytes_ext" bytes"
    total_archivos=$((total_archivos + archivos_ext))
    total_bytes=$((total_bytes + bytes_ext))
done

#reescribimos los totales de archivos y bytes

{
    echo "Total archivos: "$total_archivos""
    echo "Total bytes: "$total_bytes""

}>> "$fichSal"
exit 0