#!/bin/bash

if test $# -lt 1 || test $# -gt 2
then
	echo "Tienes que pasar minimo dos argumentos uno el fichero sin extensión y el segundo el directorio a buscar"
	exit 1
fi

nombre=$1
if test $# -eq 2
then
	if test -d $2
	then
		directorio=$2
	else
		echo "No es un directorio o no existe"
		exit 1
	fi
else
	directorio="./"
fi

find "$directorio" -type f -name "${nombre}[0-9].txt"