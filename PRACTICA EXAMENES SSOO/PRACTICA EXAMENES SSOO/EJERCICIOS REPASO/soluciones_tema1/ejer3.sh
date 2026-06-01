#!/bin/bash

# Ejercicio 3 (Tema 1 - Shell) 

# Buscar a partir del directorio como 1er argumento o del home
# los dos archivos de mas tamaño, mostrando su nombre y tamaño

if test $# -gt 1
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

cd $directorio
lista=`ls`
du -b $lista | sort -n -r| head -2 # Otra opción: find $1 -type f -size +1b -print0 | xargs -0 du -b | sort -nr |  head -2 

#Explicacion uso de los mandatos:
#--------------------------------
# find usamos -type f (tipo fichero), -size +1b (tamaño minimo 1byte -print0 (lo imprime por pantalla)
# xargs 0  (eliminamos los espacios en blanco,comillas,barras invertidas, etc)
# -print0
#TRUE; imprime el nombre completo del fichero en la salida estándar,  seguido  por  un  carácter
#nulo.  Esto  permite  que  nombres  de  ficheros que contengan saltos de línea sean correctamente
#interpretados por programas que procesen la salida de find.

#du -b -b, --bytes  (equivalent to ‘--apparent-size --block-size=1’)
#sort -nr
 #-n, --numeric-sort (compare according to string numerical value)
 #-r, --reverse (reverse the result of comparisons)

#head -2
  #-n, --lines=[-]N
  #print  the  first N lines instead of the first 10; with the leading ‘-’, print all but the last N lines of each file
