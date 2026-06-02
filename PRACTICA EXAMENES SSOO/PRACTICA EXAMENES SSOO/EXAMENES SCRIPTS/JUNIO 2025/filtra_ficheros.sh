#!/bin/bash

# Comprobación del número de argumentos
if test $# -lt 3 || test $# -gt 4
then
    echo "Uso incorrecto"
    echo "Uso: $0 directorio fichero cadena [extension]"
    exit 1
fi

# Validación de que el primer argumento es un directorio válido
if ! test -d "$1"
then
    echo "El directorio $1 no es valido o no existe"
    exit 1
fi

# Asignación de variables
directorio="$1"
fichero_salida="$2"
cadena="$3"

# Si hay 4 argumentos, usamos la extensión dada; si hay 3, la extensión por defecto es "log"
if test $# -eq 4
then
    extension="$4"
else
    extension="log"
fi

# Gestión de la salida: sobrescribimos el archivo (creándolo vacío)
> "$fichero_salida"

total_procesados=0
total_con_cadena=0

# Buscamos archivos con la extensión indicada en el directorio y subdirectorios
# Usamos un bucle while read con redirección de proceso para evitar subshells y manejar espacios
while read -r i
do
    total_procesados=$((total_procesados + 1))

    # Comprobamos si el archivo contiene la cadena
    if grep -q "$cadena" "$i"
    then
        total_con_cadena=$((total_con_cadena + 1))
        
        # Almacenamos la ruta absoluta (con realpath) del archivo
        realpath "$i" >> "$fichero_salida"
    fi

done < <(find "$directorio" -type f -name "*.$extension")

# Mostrar los mensajes requeridos por pantalla
echo "Procesando $total_procesados archivos con extensión .$extension"
echo "De los $total_procesados archivos procesados $total_con_cadena contenían \"$cadena\""

exit 0
