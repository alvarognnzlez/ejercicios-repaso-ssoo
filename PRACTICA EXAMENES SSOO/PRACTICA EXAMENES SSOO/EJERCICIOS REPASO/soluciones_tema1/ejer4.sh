#!/bin/bash

# Ejercicio 4 (Tema 1 - Shell)

# Este scrip busca los ficheros en /bin/ que NO tiene pagina de manual en: /usr/share/man/man1/nombre.1.gz
# No tiene condiciones minimas de uno o más parámetros, si no que directamente al ejecutar el script
# éste tiene que buscar lo anterior.

for i in `ls /bin`
do
	if ! test -e "/usr/share/man/man1/$i.1.gz"
	then
		echo "$i no tiene entrada de manual"
	fi	
done

# Otra opción podría ir por: 
# ls /usr/share/man/man1/ | grep -w $mandato | wc -l 
 
#Explicacion del funcionamiento:
#-------------------------------
# grep -w, --word-regexp
# Selecciona  solamente  aquellas líneas que contienen concordancias que forman palabras completas. La comprobación consiste #en que la cadena de caracteres concordante debe estar al principio de la línea o precedida por un carácter que no forme  #parte  de  una  palabra.  De  forma similar, debe estar o al final de la línea o ser seguida por un carácter no #constituyente de palabra. Los caracteres que se consideran como parte de palabras son letras, dígitos y el subrayado.

#wc -l  (cuante las lineas de la entrada estandar que reciba, desde un fichero hasta directorios que se listan)
#ls  (listamos un directorio en concreto)

#listamos el contenido del directorio man1, y buscamos la concordancia exacta del mandado con grep -w para ver si 
#existe o no... Si existe, con wc -l devolverá 1 (es decir, que SI está la pagina del manual), si no, pues devolverá 0 =)

#do
#if ! test -a "/usr/share/man/man1/$1.1.gz"
#then
#	echo "$1 NO tiene pagina de manual"
#fi
#done
