#!/bin/bash

if test $# -lt 1 || test $# -gt 2
then
	echo "Tienes que pasar minimo dos argumentos uno el argumento de entrada y otro en caso de querer guardar el resultado en uno de salida"
	exit 1
fi

if test ! -f "$1"
then
	echo "El fichero $1 no es válido o no existe"
	exit 1
fi

if test $# -eq 2
then
	salida=$2
else
	salida="default.txt"
fi

cut -d ';' -f 2,3 "$1" | sort -t ';' k1,1 > "$salida"

echo "Fichero $salida con los resultados generado perfectamente"