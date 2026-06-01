#!/bin/bash

# Ejercicio 2 (Tema 1 - Shell)

# Cogemos del 1er argumento el ejecutable que queremos buscar
# y cortando la ruta del $PATH, buscamos dicho nombre en esos directorios
# user@host:~/Escritorio$ echo $PATH
# /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games

# Ojo: tr NO CORTA, si no que sustituye. En este caso se sustituye
# los ":" por un salto de linea, para que cada ruta este en una linea.
# De esta forma, ya con un for, recorremos cada linea, la cual es una ruta
# y llamando a find $linea_ruta $1 buscamos si existe o no ese fichero 

if test $# -ne 1 #comprobamos si existe un argumento, (Nota: tb valdria poner [$# -ne 1])
then
	echo "Uso: \"$0 nombre_archivo\""
	exit 1
fi

NOMBREARCHIVO=$1
ENCONTRADO=0     #variable auxiliar para comprobar fuera del for si NO ha encontrado el fichero a encontrar
echo -e "Buscando el archivo.....\\n........................"
for RUTA_PATH in `echo $PATH | tr -s ':' '\n'` #parseamos y cambiamos : por \n salto linea
do	
	CADENA=`find $RUTA_PATH -name $NOMBREARCHIVO` #para cada linea (ruta del path) buscamos si existe ese fichero
	if ! test -z $CADENA  # if test -n "$CADENA" tambien valdria
	then
		echo "$NOMBREARCHIVO está en $CADENA"
		ENCONTRADO=1	
	fi	 
done

if test $ENCONTRADO -eq 0
then
	echo "$NOMBREARCHIVO no se encuentra en ningún directorio del PATH"
fi


