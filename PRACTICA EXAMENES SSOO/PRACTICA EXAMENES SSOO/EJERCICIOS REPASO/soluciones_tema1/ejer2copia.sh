 #!/bin/bash

if test $# -ne 1 
then
    echo "Uso \"$0 archivo\" " 
    exit 1

NOMBRE_ARCHIVO=$1
ENCONTRADO=0 #Variable que usaremos para salir del bucle

echo -e "-Buscando archivo-----------------"
for RUTA_PATH in `echo $PATH | tr -s ':' '\n'` #sustituye : por un salto de linea
do
    CADENA=`find $RUTA_PATH -name $NOMBRE_ARCHIVO` 
    if ! test -z $CADENA
    then
        echo "$NOMBRE_ARCHIVO está en $CADENA"
        ENCONTRADO=1
    fi
done
    if test $ENCONTRADO -eq 0
    then
        echo "$NOMBREARCHIVO no se ha encontrado en la ruta del PATH"
        exit 1
    fi



    