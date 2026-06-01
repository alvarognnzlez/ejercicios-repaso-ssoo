#!/bin/bash


#puede recibir uno o dos argumentos. 
#ficheroEntrada ficheroSalida.
#si no hay ficheroSalida, entonces se llama default.txt

#Juana;Borges;9

if test $# -gt 2 -o $# -lt 1
then 
    echo "Uso ficheroEntrada ficheroSalida(opcional)"
    exit 1 

fi

if ! test -f "$1"
then
    echo "El fichero $1 no es valido"
    exit 1 
fi

if test $# -eq 2
then    
    ficheroSalida="$2"
else
    ficheroSalida="default.txt"
fi

cut -d ";" -f 2,3 "$1" | sort -t ";" -k1,1 > "$ficheroSalida"

#-t ";": Le dice a sort que el delimitador entre columnas (campos) es el punto y coma ;.
#k1,1 (ojo: con coma ,, no con espacio):
# Le dice a sort que ordene utilizando únicamente la columna 1
#(desde la primera columna hasta la primera columna).