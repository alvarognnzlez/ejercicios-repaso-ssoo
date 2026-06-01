#!/bin/bash

# Ejercicio 5 (Tema 1 - Shell)

# A partir de un directorio que se le pasa como 1er argumento
# añadir permisos de ejecucion recursivamente a todos los ficheros
# con extension .sh (si no, se hace a partir del directorio actual)

if test $# -gt 1 #comprobamos si hay más de un argumento, (Nota: tb valdria poner [$# -qt 1])
then
	echo "Uso: \"$0 directorio\", o \"$0\" para buscar en el directorio actual"
	exit 1
elif test $# -eq 1
then
	if test -d $1
	then
		WDIR=$1
	else
		echo "$1 no existe o no es un directorio"
		exit 1
	fi		
else
	WDIR="./"
fi

for i in `find $WDIR -name "*.sh"`
do
	chmod u+x $i #annadimos permisos de ejecucion para usuario
done


#Otra Solucion: seria ejecutar este for al final de todo, justo aqui y asi
#-------------------------------- 
#for i in `find $1 -name "*.sh"`
#do
#	chmod u+x $1
#done
