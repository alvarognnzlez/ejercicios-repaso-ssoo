#!/bin/bash

if test $# -lt 1
then
	echo "Hay que pasarle directorio y archivos al comando"
	exit 1
elif test $# -eq 1
then
	if test -d $1
	then
		echo "No existen ficheros"
		exit 0
	else
		echo "$1 no es un directorio"
		exit 1
	fi
else
	if test -d $1
	then
		directorio=$1
		shift
	else
		echo "$1 no es un directorio"
		exit 1
	fi
fi

for fichero in $*
do
	if test -e fichero
	then
		tamano=`du -b $fichero | cut -f1`
		echo "$fichero $tamano bytes"
	elif test -e "$directorio/$fichero"
	then
		tamano=`du -b "$directorio/$fichero" | cut -f1`
		echo "$dierctorio/$fichero $tamano bytes"
	else
		echo "El fichero $fichero no existe ni en ruta absoluta ni en el directorio $directorio"
	fi
done

#Para que el resultado se escribiera en un archivo pasado como segundo argumento habría que:
#Inicializar el fichero de salida despúes de comprobar que se le pasen minimo dos argumentos. Ejemplo : salida=$2 // echo "Resultados del script filesize.sh" > "$salida"
#Y para añadir el resultado cada echo añadirlo a este fichero. Ejemplo: salida=$2	echo "$fichero $tamano bytes" >> "$salida"