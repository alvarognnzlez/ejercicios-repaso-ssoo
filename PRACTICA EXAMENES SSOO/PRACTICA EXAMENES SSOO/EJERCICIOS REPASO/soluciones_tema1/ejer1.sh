#!/bin/bash

# Ejercicio 1 (Tema 1 - Shell)

# Mostrar recursivamente todos los ficheros y directorios cuyo nombre empieze por a o b
# y que ademas no contengan el caracter ~
# Todo ello a partir de un directorio que recibe como argumento
# si no, buscar a partir del directorio de trabajo home.

if test $# -gt 1 #comprobamos si existe uno o ningún argumento, (Nota: tb valdria poner [$# -gt 1])
then
	echo "Uso: \"$0 directorio\", o \"$0\" para buscar en el directorio actual"
	exit 1
elif test $# -eq 1
then
	if test -d $1
	then
		directorio=$1
	else
		echo "$1 no existe o no es un directorio"
		exit 1
	fi		
else
	directorio="./"
fi

find $directorio -name "[ab]*"  | grep -v '~'
































