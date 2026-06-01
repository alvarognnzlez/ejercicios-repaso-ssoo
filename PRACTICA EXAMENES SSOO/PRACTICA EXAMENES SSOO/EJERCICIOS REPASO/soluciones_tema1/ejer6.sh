#!/bin/bash

# Ejercicio 6 (Tema 1 - Shell)
# Script que dado un archivo tgz elimina de su interior todos los
# archivos de tamaño superior a 8 Kbytes.

if test $# -ne 1
then
	echo "Uso: $0 archivo.tgz"
	exit 1
fi

archivo=$1

# Comprobamos si el archivo existe y es válido
if ! test -f $archivo
then
	echo "$archivo no es un archivo válido o no existe"
	exit 1
fi

# Comprobamos si el archivo tiene extensión .tgz
if ! echo "$archivo" | grep -q "\.tgz$"
then
	echo "El archivo no tiene extensión .tgz"
	exit 1
fi

# Creamos un directorio temporal para trabajar, es como un zip
temporal=$(mktemp -d)

# Descomprimimos el archivo .tgz en el directorio temporal
tar -xzf $archivo -C $temporal

# Eliminamos de su interior todos los archivos de tamaño superior a 8 Kbytes (+8k)
find $temporal -type f -size +8k -delete

# Volvemos a empaquetar el contenido restante en el archivo .tgz original
tar -czf $archivo -C $temporal .

# Limpiamos el directorio temporal
rm -rf $temporal

echo "Proceso completado: se han eliminado los archivos superiores a 8 KB de $archivo"