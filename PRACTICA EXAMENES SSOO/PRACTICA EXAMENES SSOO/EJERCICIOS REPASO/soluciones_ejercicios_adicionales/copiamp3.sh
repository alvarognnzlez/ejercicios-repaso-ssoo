#!/bin/bash

if test $# -lt 1 || test $# -gt 2
then
	echo "Tienes que pasar minimo dos argumentos uno el directorio de búsqueda y el otro el de copia si quieres"
	exit 1
fi

if test ! -d $1
then
	echo "$1 no es un directorio válido"
	exit 1
fi

if test $# -eq 2
then
	destino=$2
	if test ! -d "$destino"
	then
		echo "$2 no es un directorio válido o no existe"
		exit 1
	fi
else
	destino="./"
fi

for fichero in $(find "$1" -type f -name "*.mp3")
do
	cp "$fichero" "$destino"
done