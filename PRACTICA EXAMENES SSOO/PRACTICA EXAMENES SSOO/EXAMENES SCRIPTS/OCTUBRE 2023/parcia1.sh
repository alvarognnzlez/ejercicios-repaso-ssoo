#!/bin/bash

# a. Comprobar que el número de argumentos es correcto (1 o 2)
if test $# -lt 1 || test $# -gt 2
then
    echo "Uso incorrecto" >&2
    echo "Uso: $0 dir [file]" >&2
    exit 1
fi

# Validar que el primer argumento es un directorio
if ! test -d "$1"
then
    echo "Error: '$1' no es un directorio valido" >&2
    exit 1
fi
directorio="$1"

# Validar el segundo argumento si se proporciona
tiene_fichero=false
if test $# -eq 2
then
    if ! test -f "$2"
    then
        echo "Error: '$2' no es un fichero regular valido" >&2
        exit 1
    fi
    fichero="$2"
    tiene_fichero=true
fi

# b. Mostrar conteo recursivo de directorios, ficheros regulares y enlaces simbólicos
# -mindepth 1 en directorios evita contar el propio directorio de búsqueda
num_directorios=$(find "$directorio" -mindepth 1 -type d | wc -l)
num_regulares=$(find "$directorio" -type f | wc -l)
num_enlaces=$(find "$directorio" -type l | wc -l)

echo "=== Resumen del directorio '$directorio' ==="
echo "Numero de directorios: $num_directorios"
echo "Numero de archivos regulares: $num_regulares"
echo "Numero de enlaces simbolicos: $num_enlaces"
echo ""

# Si se ha proporcionado el fichero de comparación
if [ "$tiene_fichero" = true ]
then
    ruta_abs_fichero=$(realpath "$fichero")

    # c. Mostrar archivos con el mismo contenido
    echo "=== Archivos con el mismo contenido que '$fichero' ==="
    encontrado_contenido=false
    while read -r i
    do
        # Evitamos comparar el fichero consigo mismo
        if [ "$(realpath "$i")" != "$ruta_abs_fichero" ]
        then
            if cmp -s "$i" "$fichero"
            then
                echo "$i"
                encontrado_contenido=true
            fi
        fi
    done < <(find "$directorio" -type f)

    if [ "$encontrado_contenido" = false ]
    then
        echo "Ninguno"
    fi
    echo ""

    # d. Mostrar enlaces fuertes usando la sugerencia -samefile de find
    echo "=== Enlaces fuertes de '$fichero' ==="
    encontrado_enlace=false
    while read -r i
    do
        # Evitamos mostrar el propio fichero de comparación
        if [ "$(realpath "$i")" != "$ruta_abs_fichero" ]
        then
            echo "$i"
            encontrado_enlace=true
        fi
    done < <(find "$directorio" -samefile "$fichero")

    if [ "$encontrado_enlace" = false ]
    then
        echo "Ninguno"
    fi
fi

exit 0
